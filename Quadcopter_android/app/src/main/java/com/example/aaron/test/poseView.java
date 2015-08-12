package com.example.aaron.test;

/*
 * Copyright (C) 2011 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

        import android.content.Context;
        import android.util.AttributeSet;
        import android.widget.TextView;
        import org.ros.android.MessageCallable;
        import org.ros.message.MessageListener;
        import org.ros.namespace.GraphName;
        import org.ros.node.ConnectedNode;
        import org.ros.node.Node;
        import org.ros.node.NodeMain;
        import org.ros.node.topic.Subscriber;
        import geometry_msgs.Pose;
        import geometry_msgs.PoseStamped;
        import geometry_msgs.Point;



        import org.apache.commons.logging.Log;


/**
 * @author damonkohler@google.com (Damon Kohler)
 */
public class poseView<T> implements NodeMain {

    public String topicName;
    public String messageType,frame_id;
    public int newMeasurementFlag;
    final int maxBots=100;
    public double x,y,z,k,w,id,i,j;
    public PoseStamped pose;
    public float poseData[]={0,0,0,0,0,0,0,0,0};
    public Point p;
    public int num=0;
    public boolean newRobot=false;
    public turtle turtleList[]=new turtle[maxBots];
    //private MessageCallable<String, T> callable;

    public poseView() {
        this.y=1;
        this.z=1;
        this.x=1;
        this.k=1;
        this.w=1;
        this.i=0;
        this.j=0;
        messageType= geometry_msgs.PoseStamped._TYPE;
        for (int i=0;i<maxBots;i++){
            turtleList[i]=new turtle();
        }
    }

   public turtle[] getTurtles(){
       return turtleList;
   }

    public void setMessageType(String messageType) {
        this.messageType = messageType;
    }

    public double getX(){
        return x;
    }
    public double getY(){
        return y;
    }
    public float[] getPoseData(){return poseData;}
    public double getZ(){
        return z;
    }
    public double getID(){return id;}
    public double getYaw(){return Math.atan2(2*(w*k),(w*w-k*k))*57.2957795;}
    public Point getP(){return p;}
    public void setX(double xx) {
    x=xx;
    }
    public void setY(double xx) {
        x=xx;
    }
    /*public void setMessageToStringCallable(MessageCallable<String, T> callable) {
        this.callable = callable;
    }*/

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("poseFinder");
    }

    @Override
    public void onStart(ConnectedNode connectedNode) {
        x=-3;
        Subscriber<geometry_msgs.PoseStamped> subscriber = connectedNode.newSubscriber("poseEstimation", messageType);
        subscriber.addMessageListener(new MessageListener<PoseStamped>() {
            @Override
            public void onNewMessage(geometry_msgs.PoseStamped pose) {

                //System.out.println("found"+pose.getPosition().getX());
                //x=pose.getPosition().getX();
                newMeasurementFlag=1;
                y=pose.getPose().getPosition().getY();
                z=pose.getPose().getPosition().getZ();
                p=pose.getPose().getPosition();
                x=(pose.getPose().getPosition().getX());
                w=pose.getPose().getOrientation().getW();
                i=pose.getPose().getOrientation().getX();
                j=pose.getPose().getOrientation().getY();
                k=pose.getPose().getOrientation().getZ();
                id=(double)pose.getHeader().getSeq();
                frame_id=pose.getHeader().getFrameId();
                poseData[0]=(float)x;
                poseData[1]=(float)y;
                poseData[2]=(float)z;
                poseData[3]=-(float)(Math.atan2(-2*(i*j-w*k), w*w+i*i-j*j-k*k)*57.2957795);
                poseData[5]=0;
                poseData[6]=1;
                poseData[7]=(float)((Math.atan2(-2*(j*k-w*i), w*w-i*i-j*j+k*k))*57.2957795);
                poseData[8]=(float)((Math.asin(2 * (i * k + w * j)))*57.2957795);


                System.out.println(frame_id);
                newRobot=true;
                if (num==0){
                    num++;
                    poseData[4]=0;
                    newRobot=false;
                }
                else{
                    for (int i=0;i<num;i++){
                        if (turtleList[i].getIdentification().equals(frame_id)){
                            poseData[4]=i;
                            newRobot=false;
                        }
                    }
                }
                if (newRobot==true){
                    poseData[4]=num;
                    num++;
                }


                /*if (frame_id.equals("Bob")){poseData[4]=0;}else if(frame_id.equals("Frank")){poseData[4]=1;}
                else if(frame_id.equals("Eric")){poseData[4]=4;}
                else if(frame_id.equals("Gypsy Danger")){poseData[4]=3;}
                else if(frame_id.equals("Typhoon")){poseData[4]=2;}
                else{poseData[4]=0;}*/




                turtleList[(int)poseData[4]].setData(poseData,frame_id,0);


            }
        });
    }

    @Override
    public void onShutdown(Node node) {
    }

    @Override
    public void onShutdownComplete(Node node) {
    }

    @Override
    public void onError(Node node, Throwable throwable) {
    }
}