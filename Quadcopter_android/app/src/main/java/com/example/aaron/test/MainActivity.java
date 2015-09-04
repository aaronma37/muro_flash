package com.example.aaron.test;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;
import com.example.aaron.simplevoronoi.src.main.java.be.humphreys.simplevoronoi.*;
import org.ros.address.InetAddressFactory;
import org.ros.android.MessageCallable;
import org.ros.android.RosActivity;
import org.ros.android.view.RosTextView;
import org.ros.node.ConnectedNode;
import org.ros.node.DefaultNodeMainExecutor;
import org.ros.node.NodeConfiguration;
import org.ros.node.NodeMainExecutor;
import geometry_msgs.Point;
import geometry_msgs.Pose;

import com.example.aaron.simplevoronoi.src.main.java.be.humphreys.simplevoronoi.Voronoi;
import com.example.aaron.test.Talker;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
//import org.ros.rosjava_tutorial_pubsub.Talker;


/**
 * @author damonkohler@google.com (Damon Kohler)
 */
public class MainActivity extends RosActivity {

    private RosTextView<std_msgs.String> rosTextView;
    private Talker talker;
    private dummyMaker dummy;
    private poseView poseview;
    private MyGLSurfaceView mGLView;
    private PathPublisher pathPublisher;
    private GaussPublisher gaussPublisher;
    private pingSender pinger;
    private multipleGoalListener MultipleGoalListener;
    private allPositionsPublisher SelectedPositionsPublisher;
    private pingListener pinging;
    private float width1,height1;
    private messager message;
    final int maxBots=50;
    private int flag=0;
    private long time1=0;
    public float ping=0;
    private long time2=0;
    public Intent intent;
    public Voronoi vor;
    public double p[];
    public float pos[]={0,0,0,0,0};
    public float poseData[]={0,0,0,0,0};
    public turtle[] turtleList=new turtle[maxBots];
    public View decorView;
    private int currentApiVersion;
    //public turtle turt;
    TextView poseX;
    private List<GraphEdge> voronoiEdges;
    ArrayList<Double> temp= new ArrayList<Double>();
    ArrayList<Double> temp2= new ArrayList<Double>();




    public MainActivity() {
        super("Pubsub Tutorial", "Pubsub Tutorial");
    }

    @SuppressWarnings("unchecked")
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        final int flags = View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LOW_PROFILE;
        currentApiVersion = android.os.Build.VERSION.SDK_INT;
        if(currentApiVersion >= Build.VERSION_CODES.KITKAT)
        {
            getWindow().getDecorView().setSystemUiVisibility(flags);
            final View decorView = getWindow().getDecorView();
            decorView
                    .setOnSystemUiVisibilityChangeListener(new View.OnSystemUiVisibilityChangeListener()
                    {

                        @Override
                        public void onSystemUiVisibilityChange(int visibility)
                        {
                            if((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0)
                            {
                                decorView.setSystemUiVisibility(flags);
                            }
                        }
                    });
        }



        setContentView(R.layout.activity_main);
        View decorView = getWindow().getDecorView();

        rosTextView = (RosTextView<std_msgs.String>) findViewById(R.id.text);
        rosTextView.setTopicName("chatter");
        rosTextView.setMessageType(std_msgs.String._TYPE);
        //std_msgs.String message = "Aaron Ma";
        rosTextView.setMessageToStringCallable(new MessageCallable<String, std_msgs.String>() {
            @Override
            public String call(std_msgs.String message) {
                return message.getData();
            }
        });

        mGLView = new MyGLSurfaceView(this, pos,turtleList);
        setContentView(mGLView);
        for (int i=0;i<maxBots;i++){
            turtleList[i]=new turtle();
        }

    }

    @SuppressLint("NewApi")
    @Override
    public void onWindowFocusChanged(boolean hasFocus)
    {
        super.onWindowFocusChanged(hasFocus);
        if(currentApiVersion >= Build.VERSION_CODES.KITKAT && hasFocus)
        {
            getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE
                            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN);
        }
    }

    @Override
    protected void init(final NodeMainExecutor nodeMainExecutor) {

        double num=1;
        talker = new Talker(num);
        dummy=new dummyMaker(num);
        pathPublisher=new PathPublisher();
        gaussPublisher = new GaussPublisher();
        poseview = new poseView();
        MultipleGoalListener = new multipleGoalListener();
        SelectedPositionsPublisher= new allPositionsPublisher();
        vor = new Voronoi(.001);

        //NodeConfiguration nodeConfiguration = NodeConfiguration.newPrivate();
        final NodeConfiguration nodeConfiguration = NodeConfiguration.newPublic(InetAddressFactory.newNonLoopback().getHostAddress().toString(), getMasterUri());
        nodeConfiguration.setMasterUri(getMasterUri());
        width1=mGLView.getWidth1();
        height1=mGLView.getHeight1();
        talker=new Talker(num);
        dummy=new dummyMaker(num);
        pathPublisher=new PathPublisher();
        pinger = new pingSender();
        pinging = new pingListener();
        message = new messager();
        nodeMainExecutor.execute(poseview, nodeConfiguration);
        nodeMainExecutor.execute(dummy, nodeConfiguration);
        nodeMainExecutor.execute(pathPublisher, nodeConfiguration);


        num=poseview.getX();
        talker.setNum(num);



        ScheduledThreadPoolExecutor exec = new ScheduledThreadPoolExecutor(5);
        exec.scheduleAtFixedRate(new Runnable() {
            public void run() {
                if (poseview.newMeasurementFlag == 1) {
                    turtleList = poseview.getTurtles();
                    poseview.newMeasurementFlag = 0;

                    mGLView.updateRen(turtleList);
                    time1 = System.currentTimeMillis();
                    for (int i = 0; i < turtleList.length; i++) {
                        if (turtleList[i].getOn() == 1) {
                            turtleList[i].deltaT = (time1 - time2) / 1000;
                        }
                    }
                    time2 = time1;
                } else {
                    /*for (int i = 0; i < turtleList.length; i++) {
                        if (turtleList[i].getOn() == 1) {
                            turtleList[i].runPredictor();
                        }
                    }*/
                    //mGLView.updateRen(turtleList);
                }


            }
        }, 0, 35000, TimeUnit.MICROSECONDS);


        ScheduledThreadPoolExecutor exec2 = new ScheduledThreadPoolExecutor(5);
        exec2.scheduleAtFixedRate(new Runnable() {
            public void run() {
                if (flag == 1) {
                    mGLView.setVoronoiCoordinates();
                }
                flag = mGLView.vFlag;
                mGLView.tick();


                if (mGLView.pFlag == 1 && mGLView.pFlag2 == 1) {
                    if (talker.flag == 0) {
                        nodeMainExecutor.execute(talker, nodeConfiguration);
                    }
                    talker.setPoint(mGLView.getpX() / mGLView.getScale(), mGLView.getpY() / mGLView.getScale());
                    talker.flag = 1;
                } else {
                    nodeMainExecutor.shutdownNodeMain(talker);
                    talker.flag = 0;
                }


                if (mGLView.dummyFlag == 1) {
                    dummy.flag = 1;
                    mGLView.dummyFlag = 0;
                } else {
                    mGLView.dummyFlag = 0;
                }

                for (int i = 0; i < turtleList.length; i++) {
                    if (turtleList[i].getOn() == 1) {
                        turtleList[i].setRot();
                    }
                }
                if (mGLView.obsticle.on == 1) {
                    turtleList[49] = mGLView.obsticle;
                } else {
                    turtleList[49].x = 0;
                    turtleList[49].y = 0;
                    turtleList[49].z = 0;
                }
                mGLView.updateRen(turtleList);

            }

        }, 0, 50000, TimeUnit.MICROSECONDS);



        ScheduledThreadPoolExecutor exec3 = new ScheduledThreadPoolExecutor(5);
        exec3.scheduleAtFixedRate(new Runnable() {
            public void run() {
                if (mGLView.pathPublisherFlag==true){
                    pathPublisher.active=1;
                    pathPublisher.setPathArray(mGLView.passPathArray());
                    pathPublisher.flag=true;
                    mGLView.pathPublisherFlag=false;
                }
                else{
                    pathPublisher.active=0;
                }


                /*if (mGLView.getActive()==true){
                    message.setMethod(1);
                    if (mGLView.newAction==true){
                        message.active=true;
                        nodeMainExecutor.execute(message, nodeConfiguration);
                        mGLView.newAction=false;
                    }

                    *//*if (message.active==false){
                        nodeMainExecutor.shutdownNodeMain(message);
                    }*//*

                }
                else{
                    message.setMethod(0);
                    if (mGLView.newAction==true){
                        message.active=true;
                        nodeMainExecutor.execute(message, nodeConfiguration);
                        mGLView.newAction=false;
                    }

                    *//*if (message.active==false){
                        nodeMainExecutor.shutdownNodeMain(message);
                    }*//*
                }*/

                if (mGLView.getActive()==true){

                    if(SelectedPositionsPublisher.active==0){
                        nodeMainExecutor.execute(SelectedPositionsPublisher, nodeConfiguration);
                        //nodeMainExecutor.execute(MultipleGoalListener, nodeConfiguration);
                    }
                    SelectedPositionsPublisher.active=1;
                    SelectedPositionsPublisher.setPositions(turtleList);
                    SelectedPositionsPublisher.flag=true;
                    MultipleGoalListener.flag =true;
                    mGLView.setCentroids(MultipleGoalListener.dummyArray);
                }
                else{
                    SelectedPositionsPublisher.active=0;
                    nodeMainExecutor.shutdownNodeMain(SelectedPositionsPublisher);
                    nodeMainExecutor.shutdownNodeMain(MultipleGoalListener);
                }

                if (mGLView.gFlag==1){
                    if(gaussPublisher.active==0){
                        gaussPublisher.getGaussData(mGLView.getGausses());
                        nodeMainExecutor.execute(gaussPublisher, nodeConfiguration);
                        gaussPublisher.active=1;
                    }
                }
                else{
                    //nodeMainExecutor.shutdownNodeMain(gaussPublisher);
                }


            }
        }, 0, 50000, TimeUnit.MICROSECONDS);


      /*  nodeMainExecutor.execute(pinger, nodeConfiguration);
        nodeMainExecutor.execute(pinging, nodeConfiguration);
        ScheduledThreadPoolExecutor exec4 = new ScheduledThreadPoolExecutor(6);
        exec4.scheduleAtFixedRate(new Runnable() {
            public void run() {
                pinger.flag=true;
                pinging.received=false;

                //while(pinger.flag==true){}


                //while (pinging.received==false){}
                ping=pinging.timeEnd -pinger.timeStart;
                mGLView.updatePing(ping);

            }
        }, 0, 500000, TimeUnit.MICROSECONDS);*/

    }
}