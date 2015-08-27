package com.example.aaron.test;
/*
 * Copyright (C) 2011 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

        import android.app.Activity;
        import com.example.aaron.simplevoronoi.src.main.java.be.humphreys.simplevoronoi.*;
        import android.content.Context;
        import android.graphics.Point;
        import android.opengl.GLSurfaceView;
        import android.util.DisplayMetrics;
        import android.view.Display;
        import android.view.MotionEvent;
        import android.view.View;
        import android.view.WindowManager;
        import android.os.Vibrator;

        import java.math.BigDecimal;
        import java.text.DecimalFormat;
        import java.util.ArrayList;
        import java.util.List;
        import java.util.concurrent.ScheduledThreadPoolExecutor;
        import java.util.concurrent.TimeUnit;


/**
 * A view container where OpenGL ES graphics can be drawn on screen.
 * This view can also be used to capture touch events, such as a user
 * interacting with drawn objects.
 */
public class MyGLSurfaceView extends GLSurfaceView {


    private final MyGLRenderer mRenderer;
    public float poseData[];
    final int maxBots=50;
    public turtle tList[]=new turtle[maxBots];
    public turtle obsticle = new turtle();
    private float width1;
    private float height1;
    private float pX=0;
    private float mapLeft,mapTop,mapBottom,workspace;
    private float pY=0;
    private Vibrator v;
    public int vFlag=0;
    public int dummyFlag=0;
    private int connectable=0;
    private float firstPointFreeDraw[] ={-1000,-1000};
    private float tempTurtleList[]= new float[9];
    public int fFlag=0;
    public int gFlag = 0;
    public int gFlag2 = 0;
    public int gpFlag=0;
    private int count=0;
    private int i;
    public int pFlag=0;
    private boolean recievedTurtles=false;
    public int pFlag2=0;
    public boolean pathPublisherFlag=false;
    public int antispam=0;

    private int freeDrawCount=0;
    private int gaussDrawCount=0;
    private float vorCoords[] = {
            -0.5f,  0.5f, 0.0f,   // top left
            -0.5f, -0.5f, 0.0f,   // bottom left
            0.5f, -0.5f, 0.0f,   // bottom right
            0.5f,  0.5f, 0.0f }; // top right
    private double cd;
    private double cy;




    ArrayList<Double> temp= new ArrayList<Double>();
    ArrayList<Double> temp2= new ArrayList<Double>();

    private int gInd = 0;
    private float gaussScale;


    public Voronoi vor;
    private List<GraphEdge> voronoiEdges;
    private int state[]=new int[maxBots];
    public MyGLSurfaceView(Context context, float f[], turtle turtleList[]) {
        super(context);



        v = (Vibrator) context.getSystemService(Context.VIBRATOR_SERVICE);
        vor = new Voronoi(.001);
        DisplayMetrics metrics = new DisplayMetrics();
        ((Activity) getContext()).getWindowManager().getDefaultDisplay().getMetrics(metrics);
        width1 = metrics.widthPixels;
        height1 = metrics.heightPixels;
        mapLeft=(width1-100)/height1;
        mapTop=(height1-5)/height1;
        mapBottom=-(height1-5)/height1;

        poseData=f;
        // Create an OpenGL ES 2.0 context.
        setEGLContextClientVersion(2);
        for (int i=0;i<maxBots;i++){
            tList[i]=new turtle();
            state[i]=0;
        }

        // Set the Renderer for drawing on the GLSurfaceView
        mRenderer = new MyGLRenderer(context,f, tList,width1,height1);
        //float posTemp[]=f;
        setEGLConfigChooser(new MultisampleConfigChooser());
        setRenderer(mRenderer);

        // Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);

    }

    public void updateRen(turtle[] t){
        for (int i=0;i<maxBots;i++) {
                tempTurtleList=t[i].getData();
                tempTurtleList[5]=state[i];
                tList[i].setData(tempTurtleList, t[i].getIdentification(), t[i].getType());
        }
        mRenderer.updateRen(tList);
    }

    public float getHeight1(){
        return height1;
    }

    public void tick(){
        antispam=antispam+1;
       count=0;
        for (int i=0;i<15;i++){
            if (tList[i].getState()==1){
                count++;
                if (count>1){
                    mRenderer.textList.get(mRenderer.ardroneTextBegin).setText("Multiple Robots Selected");
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+2).setText(" X:");
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+3).setText(" Y:");
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+4).setText(" Z:");
                }
                else{
                    mRenderer.textList.get(mRenderer.ardroneTextBegin).setText(tList[i].getIdentification());
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+2).setText(" X:" + truncateDecimal(tList[i].getX(),3));
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+3).setText(" Y:" + truncateDecimal(tList[i].getY(),3));
                    mRenderer.textList.get(mRenderer.ardroneTextBegin+4).setText(" Z:" + truncateDecimal(tList[i].getZ(),3));
                }
            }
        }
        if (count==0){
            mRenderer.textList.get(mRenderer.ardroneTextBegin).setText("No Robots Selected");
            mRenderer.textList.get(mRenderer.ardroneTextBegin+2).setText(" X:");
            mRenderer.textList.get(mRenderer.ardroneTextBegin+3).setText(" Y:");
            mRenderer.textList.get(mRenderer.ardroneTextBegin+4).setText(" Z:");
        }

    }


    public float getWidth1(){
        return width1;
    }

    private final float TOUCH_SCALE_FACTOR = 180.0f / 320;
    private float mPreviousX;
    private float mPreviousY;
    private float previousx = 0;
    private float previousy = 0;
    public void setR(float f[]){mRenderer.setPosition(f);}

    @Override
    public boolean onTouchEvent(MotionEvent e) {

        float x = e.getX();
        float y = e.getY();

        int cc=0;

        float xGL=(width1/2-x)/(float)(height1/1.85);
        float yGL=( height1/2+30-y)/(float)(height1/1.85);

        workspace=-(width1-115)/(height1*2)+.15f-mRenderer.slider;


        mRenderer.tempFun(xGL, yGL);

        switch (e.getActionMasked()) {
            case MotionEvent.ACTION_DOWN:
                mRenderer.tToggle=1;

                for (int i=0;i<maxBots;i++){
                    if(tList[i].getOn()==1) {
                        cc=cc+1;
                        if (Math.abs(tList[i].getX()*getScale() - xGL) < .1f && Math.abs(tList[i].getY()*getScale() - yGL) < .1f) {
                            if (state[i] == 0) {
                                state[i]=1;
                            } else {
                                state[i]=0;
                            }
                            v.vibrate(50);
                        }
                    }
                }

                if (antispam>1) {
                    //Turn on voronoi toggle
                    if (xGL<-(width1-115)/(height1*2)-.01f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.11f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                    if (mRenderer.getvToggle() == 1) {
                        mRenderer.setvToggle(0);
                        v.vibrate(500);
                    } else {
                        mRenderer.setvToggle(1);
                    }
                    vFlag = mRenderer.getvToggle();


                    //Turn on Free Draw Toggle
                    if (xGL<-(width1-115)/(height1*2)-.12f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.22f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f)
                        if (mRenderer.getfToggle() == 1) {
                            mRenderer.setfToggle(0);
                            mRenderer.eraseFreeLine();
                            freeDrawCount=0;
                            previousy=0;
                            previousx=0;
                            fFlag = mRenderer.getfToggle();
                            v.vibrate(50);
                        } else {
                            mRenderer.setfToggle(1);
                            connectable=0;
                            firstPointFreeDraw[0]=-1000;
                            firstPointFreeDraw[1]=-1000;
                            fFlag = mRenderer.getfToggle();
                            v.vibrate(50);
                        }

                    //Turn on Way Point Toggle
                    if (xGL<-(width1-115)/(height1*2)-.23f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.32f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                        if (mRenderer.getpToggle() == 1) {
                            mRenderer.setpToggle(0);
                            pFlag2=0;
                            mRenderer.setpToggle2(pFlag2);
                            mRenderer.tToggle=1;
                            pX=0; pY=0;
                            v.vibrate(50);
                        } else {
                            mRenderer.tToggle=0;
                            mRenderer.setpToggle(1);
                            pX=0; pY=0;
                            v.vibrate(50);
                        }
                    pFlag = mRenderer.getpToggle();

                    //Turn on AndroneToggle
                    if (xGL<-(width1-115)/(height1*2)-.34f-mRenderer.slider && xGL>-(width1 -115)/(height1*2)-.43f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                        if (mRenderer.getAPToggle() == 1) {
                            mRenderer.setAPToggle(0);
                            v.vibrate(50);
                        } else {
                            mRenderer.setAPToggle(1);
                            v.vibrate(50);
                        }

                    if (xGL<-(width1-115)/(height1*2)-.45f-mRenderer.slider && xGL>-(width1 -115)/(height1*2)-.54f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                    {
                        dummyFlag=1;
                        v.vibrate(50);
                    }

                    //IF GAUSS TOGGLE SELECTED
                    if (xGL<-(width1-115)/(height1*2)-.56f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.65f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                        if (mRenderer.getgToggle() == 1) {
                            mRenderer.setgToggle(0);
                            gFlag2 = 0;
                            mRenderer.setgToggle2(gFlag2);
                            //mRenderer.tToggle=1;
                            v.vibrate(50);
                        } else {
                            //mRenderer.tToggle=0;

                            mRenderer.setgToggle(1);

                            v.vibrate(50);
                        }
                    gFlag = mRenderer.getgToggle();

                    //IF GAUSSPATH TOGGLE SELECTED
                    if (xGL<-(width1-115)/(height1*2)-.66f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.76f-mRenderer.slider && yGL > (height1)/(height1)-.2f && yGL < (height1)/(height1)-.1f )
                        if (mRenderer.getgpToggle() == 1) {
                            mRenderer.setgpToggle(0);
                            mRenderer.eraseGaussLine();
                            v.vibrate(50);
                        } else {
                            mRenderer.setgpToggle(1);
                            mRenderer.makeGaussPoints();
                            v.vibrate(50);
                        }
                    gpFlag = mRenderer.getgpToggle();

                    //Toggle for voronoi deployment
                    if (xGL<-(width1-115)/(height1*2)-.01f-mRenderer.slider&& xGL>-(width1-115)/(height1*2)-.1f-mRenderer.slider&& yGL > (height1)/(height1)-.3f && yGL < (height1)/(height1)-.2f)
                        if (mRenderer.voronoiDeploymentToggle.active == true) {
                            mRenderer.voronoiDeploymentToggle.active =  false;
                            v.vibrate(50);
                        } else {
                            mRenderer.voronoiDeploymentToggle.active = true;
                            v.vibrate(50);
                        }

                    if (xGL<-(width1-115)/(height1*2)-.12f-mRenderer.slider && xGL>-(width1-115)/(height1*2)-.22f-mRenderer.slider && yGL >(height1)/(height1)-.3f&& yGL < (height1)/(height1)-.2f)
                        if (mRenderer.dragToggle.active == true) {
                            mRenderer.dragToggle.active =  false;
                            obsticle.on=1;
                            v.vibrate(50);
                        } else {
                            mRenderer.dragToggle.active = true;
                            obsticle.on=0;
                            v.vibrate(50);
                        }


/*                    if (xGL<-(width1-90)/height1+.05f && xGL>-(width1-90)/height1 && yGL >-(height1-10)/(height1)-mRenderer.slider  && yGL < -(height1-10)/(height1)+05f-mRenderer.slider ){

                    }*/

                    if (xGL< .85 && xGL>.65 && yGL > -.95 && yGL < -.75 ){
                            mRenderer.scale=mRenderer.scale+.5f;
                            mRenderer.textList.get(1).setText("Scale: "+truncateDecimal(mRenderer.scale,1)+"x");
                            mRenderer.textList.get(2).setText(truncateDecimal(2 / mRenderer.scale, 2) + " ft");
                            v.vibrate(75);
                        }
                    if (xGL< 1.2f && xGL>.95f && yGL > -.85f && yGL < -.75 && mRenderer.scale>.5f){
                        mRenderer.scale=mRenderer.scale-.5f;
                        mRenderer.textList.get(1).setText("Scale: "+truncateDecimal(mRenderer.scale,1)+"x");
                        mRenderer.textList.get(2).setText( truncateDecimal(2 / mRenderer.scale,2) + " ft");
                        v.vibrate(75);
                    }

                    //Clear button
                    if (xGL< -1.05f && xGL> -1.35f && yGL > -.9f && yGL < -.65f && mRenderer.scale>.5f){
                        gInd = 0;
                        mRenderer.clearGauss();
                        v.vibrate(75);
                    }

                }





                if (pFlag==1 && xGL>workspace  && xGL < mapLeft && yGL < mapTop && yGL > mapBottom){
                    pX=xGL;
                    pY=yGL;
                    pFlag2=1;
                    mRenderer.setpToggle2(pFlag2);
                    mRenderer.setWayPointValues(pX,pY);
                }

                if (gFlag==1 && xGL>workspace  && xGL < mapLeft && yGL < mapTop && yGL > mapBottom){
                    //pX=xGL;
                    //pY=yGL;
                    gFlag2=1;
                    mRenderer.setgToggle2(gFlag2);

                    //System.out.println("GAUSSIAN INDEX: " +gInd);
                    //mRenderer.setWayPointValues(xGL, yGL);
                    //mRenderer.setGaussValues(xGL, yGL, gInd);
                    //mRenderer.setGaussScale(1f);
                    if (gInd<99){
                        mRenderer.addGaussStuff(xGL, yGL, 1f,gInd);
                    }
                    gInd++;
                }



                /*if (yGL > -(height1)/(height1*2)-mRenderer.slider && yGL < -(height1-100)/(height1*2)-mRenderer.slider &&xGL>-(-200)/height1 && xGL < (200)/height1){
                    grab=1;
                }
                else{
                    grab=0;
                }*/

            case MotionEvent.ACTION_POINTER_DOWN:
                //System.out.println("GAUSS 0");
                if (e.getActionIndex() == 1) {


                    if(gFlag==1){
                        float gaussX = e.getX(1);

                        float gaussY = e.getY(1);


                        float gauss_xGL = (width1 / 2 - gaussX) / (float) (height1 / 1.85);
                        float gauss_yGL = (height1 / 2 + 30 - gaussY) / (float) (height1 / 1.85);

                        float gauss_dx = gauss_xGL - xGL;
                        float gauss_dy = gauss_yGL - yGL;

                        float dgauss = (float)Math.sqrt(Math.pow(gauss_dx, 2)+ Math.pow(gauss_dy, 2));

                        gaussScale = dgauss/.2f;
                        //System.out.println("SCALE");
                        //mRenderer.addGaussStuff(xGL, yGL, gaussScale,gInd-1);
                        mRenderer.setGaussScale(gInd-1, gaussScale);
                    }
                }


            case MotionEvent.ACTION_MOVE:

                mRenderer.tToggle=1;
                float dx = x - mPreviousX;
                float dy = y - mPreviousY;


                    if (mRenderer.dragToggle.active==true && xGL>workspace
                    && xGL < mapLeft && yGL < mapTop  && yGL > mapBottom){
                        obsticle.x=xGL/getScale();
                        obsticle.y=yGL/getScale();
                        obsticle.Aw=1;
                    }



                if (fFlag==1 && (Math.abs(xGL-previousx)> .03f || Math.abs(yGL -previousy)>.03f) && xGL>workspace
                        && xGL < mapLeft && yGL < mapTop  && yGL > mapBottom) {
                    if (previousx!=0 && previousy!=0){
                        if (firstPointFreeDraw[0]==-1000){
                            firstPointFreeDraw[0]=previousx;
                            firstPointFreeDraw[1]=previousy;
                        }
                        else if (xGL > firstPointFreeDraw[0]+.1f || xGL < firstPointFreeDraw[0]-.1f || yGL > firstPointFreeDraw[1]+.1f || yGL < firstPointFreeDraw[1]-.1f){
                            connectable=1;
                        }
                        else if (connectable==1){
                            fFlag=0;
                            setFreeDrawCoordinates(firstPointFreeDraw[0],firstPointFreeDraw[1], previousx, previousy,true);
                            v.vibrate(50);
                        }

                        if (fFlag == 1) {
                            setFreeDrawCoordinates(xGL,yGL, previousx, previousy,false);
                        }

                    }

                    previousx=xGL;
                    previousy=yGL;
                }


                // reverse direction of rotation above the mid-line
                if (y > getHeight() / 2) {
                    dx = dx * -1 ;
                }

                // reverse direction of rotation to left of the mid-line
                if (x < getWidth() / 2) {
                    dy = dy * -1 ;
                }

                if (yGL > -.3f && yGL < .3f && xGL>-(width1-115)/(height1*2)-mRenderer.slider-.3f && xGL < -(width1-115)/(height1*2)-mRenderer.slider+.3f){
                    mRenderer.slider=-(width1-115)/(height1*2)-xGL;
                    if (mRenderer.slider<.1f){
                        mRenderer.slider=0;
                    }
                    else if (mRenderer.slider>-.1f+(width1-115)/(height1*2)) {
                        mRenderer.slider=(width1-115)/(height1*2);
                    }
                }


                mRenderer.setAngle(
                        mRenderer.getAngle() +
                                ((dx + dy) * TOUCH_SCALE_FACTOR));  // = 180.0f / 320
                requestRender();


                if (pFlag==1 && xGL>workspace  && xGL < mapLeft && yGL < mapTop && yGL > mapBottom){
                    pX=xGL;
                    pY=yGL;
                    pFlag2=1;
                    mRenderer.setpToggle2(pFlag2);
                    mRenderer.setWayPointValues(pX,pY);
                }


                //moving gauss
                if (gFlag==1 && xGL>workspace  && xGL < mapLeft && yGL < mapTop && yGL > mapBottom){
                    pX=xGL;
                    pY=yGL;
                    gFlag2=1;
                    mRenderer.setpToggle2(gFlag2);
                    mRenderer.updateGauss(xGL, yGL, gInd-1);
                }


            case MotionEvent.ACTION_UP:

                mRenderer.tToggle=1;
        }



        mPreviousX = x;
        mPreviousY = y;

        antispam=0;
        return true;
    }

    public void setVoronoiCoordinates(){

        temp.clear();
        temp2.clear();
        for (int i=0;i<maxBots;i++){
            if (tList[i].getOn()==1) {
                temp.add((double) tList[i].getX());
                temp2.add((double) tList[i].getY());
                recievedTurtles=true;
            }
        }
        if (temp!=null && recievedTurtles==true) {

            double[] temp3 = new double[temp.size()];
            double[] temp4 = new double[temp.size()];
            for (int i=0;i<temp.size();i++){
                temp3[i]=temp.get(i);
                temp4[i]=temp2.get(i);
            }

            voronoiEdges = vor.generateVoronoi(temp3, temp4, -width1 / height1, width1 / height1, -height1 / (height1), height1 / height1);
            for(int i = 0; i < voronoiEdges.size(); i++) {
                cd = Math.cos(Math.atan((voronoiEdges.get(i).x1 - voronoiEdges.get(i).x2) / (voronoiEdges.get(i).y1 - voronoiEdges.get(i).y2)));
                cy = Math.sin(Math.atan((voronoiEdges.get(i).x1 - voronoiEdges.get(i).x2) / (voronoiEdges.get(i).y1 - voronoiEdges.get(i).y2)));

                vorCoords[0] = (float) voronoiEdges.get(i).x1 + (float) cd * .005f/mRenderer.scale;
                vorCoords[1] = (float) voronoiEdges.get(i).y1 - (float) cy * .005f/mRenderer.scale;

                vorCoords[9] = (float) voronoiEdges.get(i).x2 + (float) cd * .005f/mRenderer.scale;
                vorCoords[10] = (float) voronoiEdges.get(i).y2 - (float) cy * .005f/mRenderer.scale;


                vorCoords[3] = (float) voronoiEdges.get(i).x1 - (float) cd * .005f/mRenderer.scale;
                vorCoords[4] = (float) voronoiEdges.get(i).y1 + (float) cy * .005f/mRenderer.scale;

                vorCoords[6] = (float) voronoiEdges.get(i).x2 - (float) cd * .005f / mRenderer.scale;
                vorCoords[7] = (float) voronoiEdges.get(i).y2 + (float) cy * .005f/mRenderer.scale;

                mRenderer.setVoronoiCoordinates(vorCoords, i, voronoiEdges.size());
            }
        }
    }


    public void setFreeDrawCoordinates(float x, float y, float xp, float yp,boolean closed){

        float Coords[] = {
                -0.5f,  0.5f, 0.0f,   // top left
                -0.5f, -0.5f, 0.0f,   // bottom left
                0.5f, -0.5f, 0.0f,   // bottom right
                0.5f,  0.5f, 0.0f }; // top right

               double cd = Math.cos(Math.atan((x - xp) / (y - yp)));
                double cy = Math.sin(Math.atan((x - xp) / (y - yp)));

                Coords[0] = x + (float) cd * .005f;
                Coords[0]=Coords[0]/mRenderer.scale;
                Coords[1] = y - (float) cy * .005f;
                Coords[1]=Coords[1]/mRenderer.scale;


                Coords[9] = xp + (float) cd * .005f;
                Coords[9]=Coords[9]/mRenderer.scale;
                Coords[10] =  yp - (float) cy * .005f;
                Coords[10]=Coords[10]/mRenderer.scale;

                Coords[3] = x - (float) cd * .005f;
                Coords[3]=Coords[3]/mRenderer.scale;
                Coords[4] = y + (float) cy * .005f;
                Coords[4]=Coords[4]/mRenderer.scale;

                Coords[6] = xp - (float) cd * .005f;
                Coords[6]=Coords[6]/mRenderer.scale;
                Coords[7] = yp + (float) cy * .005f;
                Coords[7]=Coords[7]/mRenderer.scale;

        freeDrawCount++;
                if (freeDrawCount<100){
                    mRenderer.setFreeDrawCoordinates(Coords,freeDrawCount-1,freeDrawCount,x,y,closed);
                }
        pathPublisherFlag=true;

    }




    public float getpX(){
        return pX;
    }


    public float getpY(){
        return pY;
    }

    public float[] getGaussX() {
        return mRenderer.getGaussX();
    }
    public float[] getGaussY() {
        return mRenderer.getGaussY();
    }
    public float[] getGaussScale() {
        return mRenderer.getGaussScale();
    }



    public dummyPoseArray passPathArray(){
        return mRenderer.pathArray;
    }

    public float getScale(){
        return mRenderer.scale;
    }

    public boolean getActive(){
        return mRenderer.voronoiDeploymentToggle.active;
    }

    private static BigDecimal truncateDecimal(float x,int numberofDecimals)
    {
        if ( x > 0) {
            return new BigDecimal(String.valueOf(x)).setScale(numberofDecimals, BigDecimal.ROUND_HALF_UP);
        } else {
            return new BigDecimal(String.valueOf(x)).setScale(numberofDecimals, BigDecimal.ROUND_HALF_DOWN);
        }
    }

    public void setCentroids(dummyPoseArray tempArray){
        mRenderer.centroids=tempArray;
    }

    public boolean getObsticleActivity(){
        return mRenderer.dragToggle.active;
    }


}

