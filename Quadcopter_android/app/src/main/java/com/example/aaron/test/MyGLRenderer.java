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

        import javax.microedition.khronos.egl.EGLConfig;
        import javax.microedition.khronos.opengles.GL10;

        import android.content.Context;
        import android.opengl.GLES20;
        import android.opengl.GLSurfaceView;
        import android.opengl.Matrix;
        import android.util.Log;

        import java.nio.FloatBuffer;
        import java.util.ArrayList;
        import java.util.Collections;
        import java.util.List;

/**
 * Provides drawing instructions for a GLSurfaceView object. This class
 * must override the OpenGL ES drawing lifecycle methods:
 * <ul>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onSurfaceCreated}</li>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onDrawFrame}</li>
 *   <li>{@link android.opengl.GLSurfaceView.Renderer#onSurfaceChanged}</li>
 * </ul>
 */
public class MyGLRenderer implements GLSurfaceView.Renderer {

    private static final String TAG = "MyGLRenderer";
    private Triangle mTriangle, robot;
    private Square   mSquare, mArena, mArena2;
    private interfaceImage interfacePull;
    private Square vLine[] = new Square[25];
    private Square fLine[] = new Square[100];
    private waypoint wp;
    private grid myGrid;
    private turtB turt1;
    private ardroneImage myAr;
    private target tar;
    private origin Origin;
    private buttons plus, minus;
    private gauss density;
    public float slider=0;
    //private ArrayList<textclass> textSystem= new ArrayList<textclass>();
    private textclass textSystem;
    private toggles vorToggle, freeDrawToggle,wayPointToggle,exit,ardronePrefToggle;
    private float textPosition[]= {-.95f, .5f};
    public ArrayList<toText> textList = new ArrayList<toText>();
    private FloatBuffer textureBuffer;
    public Context context;

    private int vToggle=0;
    private int fToggle=0;
    private int APToggle=0;
    public float scale=2f;
    private int gToggle=0;
    private int pToggle=0;
    private int pToggle2=0;
    private int framecounter=0;
    public int tToggle=0;
    private float pX=0;
    private float pY=0;
    private int vSize=0;
    private int ardroneTextBegin=0;
    private int ardroneTextEnd=0;
    private int fSize=0;
    static private float newline=-.05f;
    static private float tab    =-.2f;
    private float texture[] = {
            0.0f, 1.0f,     // top left     (V2)
            0.0f, 0.0f,     // bottom left  (V1)
            1.0f, 1.0f,     // top right    (V4)
            1.0f, 0.0f      // bottom right (V3)
    };

    // mMVPMatrix is an abbreviation for "Model View Projection Matrix"
    private final float[] mMVPMatrix = new float[16];
    private final float[] mProjectionMatrix = new float[16];
    private final float[] mViewMatrix = new float[16];
    private final float[] mRotationMatrix = new float[16];
    float poseData[]={0,0,0,0,0};
    public turtle turtleList[]= new turtle[10];
    private float tempX;
    private float tempY;
    private float width,height;

    private float mAngle;

    public MyGLRenderer(Context context1,float f[], turtle t[],float w, float h){
        width=w;
        height=h;
        poseData=f;context=context1;
    for (int i=0;i<10;i++){
        turtleList[i]=new turtle();
        if (t[i]!=null){
        turtleList[i].setData(t[i].getData(), t[i].getIdentification(), t[i].getType());}
    }


    }

    public void updateRen(turtle t[]){
        for (int i=0;i<10;i++){
            if (t[i]!=null){
                turtleList[i].setData(t[i].getData(), t[i].getIdentification(), t[i].getType());}
        }
    }

    @Override
    public void onSurfaceCreated(GL10 unused, EGLConfig config) {

        // Set the background frame color
        GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        mTriangle = new Triangle();

        float sTemp[] = {
                -0.5f,  0.5f, 0.0f,   // top left
                -0.5f, -0.5f, 0.0f,   // bottom left
                0.5f, -0.5f, 0.0f,   // bottom right
                0.5f,  0.5f, 0.0f };
        float c[] = { 0f,255f,255f, 1.0f };
        mSquare   = new Square();
        Origin    = new origin(context);
        sTemp[0]=-width/height;sTemp[1]=height/height;
        sTemp[3]=-width/height;;sTemp[4]=-height/(height*2);
        sTemp[6]=width/height;;sTemp[7]=-height/(height*2);
        sTemp[9]=width/height;sTemp[10]=height/height;
        mArena  = new Square(sTemp);
        sTemp[0]=-(width-100)/height;sTemp[1]=(height-5)/height;
        sTemp[3]=-(width-100)/height;sTemp[4]=-(height-10)/(height*2);
        sTemp[6]=(width-100)/height;;sTemp[7]=-(height-10)/(height*2);
        sTemp[9]=(width-100)/height;sTemp[10]=(height-5)/height;

        c[0]=0;c[1]=0;c[2]=0;c[3]=.2f;

        mArena2  = new Square(sTemp);



        wp= new waypoint(context);
        myAr=new ardroneImage(context);
        mArena2.setColor(c);
        robot = new Triangle();
        plus= new buttons(context,0);
        minus= new buttons(context,0);


        turt1 = new turtB(context);
        tar =new target(context);
        //density = new gauss(context);

        sTemp[0]=-(width-100)/height;sTemp[1]=0;
        sTemp[3]=-(width-100)/height;sTemp[4]=-.01f;
        sTemp[6]=(width-100)/height;;sTemp[7]=-.01f;
        sTemp[9]=(width-100)/height;sTemp[10]=0;

        for (int i=0;i<25;i++) {
            vLine[i] = new Square(sTemp);
        }

        for (int i=0;i<100;i++) {
            fLine[i] = new Square(sTemp);
        }

        float spriteCoords[] = {
                -0.05f,  0.05f,   // top left
                -0.05f, -0.05f,   // bottom left
                0.05f, -0.05f,   // bottom right
                0.05f,  0.05f}; //top right

        /*spriteCoords[0]=-(width-115)/(height);spriteCoords[1]=-(height-10)/(height*2);
        spriteCoords[2]=-(width-115)/(height);spriteCoords[3]=-(height-10)/(height*2)-.1f;
        spriteCoords[4]=-(width-115)/(height)+.1f;spriteCoords[5]=-(height-10)/(height*2)-.1f;
        spriteCoords[6]=-(width-115)/(height)+.1f;spriteCoords[7]=-(height-10)/(height*2);*/

        spriteCoords[0]=-(width-115)/(height*2)-.11f;spriteCoords[1]=(height)/(height)-.1f;
        spriteCoords[2]=-(width-115)/(height*2)-.11f;spriteCoords[3]=(height)/(height)-.2f;
        spriteCoords[4]=-(width-115)/(height*2)-.01f;spriteCoords[5]=(height)/(height)-.2f;
        spriteCoords[6]=-(width-115)/(height*2)-.01f;spriteCoords[7]=(height)/(height)-.1f;
        vorToggle = new toggles(context,spriteCoords,0);

        spriteCoords[0]=-(width-115)/(height*2)-.22f;spriteCoords[1]=(height)/(height)-.1f;
        spriteCoords[2]=-(width-115)/(height*2)-.22f;spriteCoords[3]=(height)/(height)-.2f;
        spriteCoords[4]=-(width-115)/(height*2)-.12f;spriteCoords[5]=(height)/(height)-.2f;
        spriteCoords[6]=-(width-115)/(height*2)-.12f;spriteCoords[7]=(height)/(height)-.1f;
        freeDrawToggle = new toggles(context, spriteCoords,1);

        spriteCoords[0]=-(width-115)/(height*2)-.33f;spriteCoords[1]=(height)/(height)-.1f;
        spriteCoords[2]=-(width-115)/(height*2)-.33f;spriteCoords[3]=(height)/(height)-.2f;
        spriteCoords[4]=-(width-115)/(height*2)-.23f;spriteCoords[5]=(height)/(height)-.2f;
        spriteCoords[6]=-(width-115)/(height*2)-.23f;spriteCoords[7]=(height)/(height)-.1f;
        wayPointToggle = new toggles(context, spriteCoords,2);

        spriteCoords[0]=-(width-115)/(height*2)-.44f;spriteCoords[1]=(height)/(height)-.1f;
        spriteCoords[2]=-(width-115)/(height*2)-.44f;spriteCoords[3]=(height)/(height)-.2f;
        spriteCoords[4]=-(width-115)/(height*2)-.34f;spriteCoords[5]=(height)/(height)-.2f;
        spriteCoords[6]=-(width-115)/(height*2)-.34f;spriteCoords[7]=(height)/(height)-.1f;
        ardronePrefToggle = new toggles(context, spriteCoords,5);

        spriteCoords[0]=-(width-115)/(height*2)-.11f;spriteCoords[1]=(height)/(height);
        spriteCoords[2]=-(width-115)/(height*2)-.11f;spriteCoords[3]=(height)/(height)-.1f;
        spriteCoords[4]=-(width-115)/(height*2)-.01f;spriteCoords[5]=(height)/(height)-.1f;
        spriteCoords[6]=-(width-115)/(height*2)-.01f;spriteCoords[7]=(height)/(height);
        exit = new toggles(context, spriteCoords,4);

        spriteCoords[0]=-(width-115)/(height*2);spriteCoords[1]=(height)/(height);
        spriteCoords[2]=-(width-115)/(height*2);spriteCoords[3]=-(height)/(height);
        spriteCoords[4]=-(width-115)/(height*2)+.05f;spriteCoords[5]=-(height)/(height);
        spriteCoords[6]=-(width-115)/(height*2)+.05f;spriteCoords[7]=(height)/(height);

        /*spriteCoords[0]=-(width-100)/(height)+0f;spriteCoords[1]=(height)/(height);
        spriteCoords[2]=-(width-100)/(height)+0f;spriteCoords[3]=-(height)/(height);
        spriteCoords[4]=-(width-100)/(height)+.05f;spriteCoords[5]=-(height)/(height);
        spriteCoords[6]=-(width-100)/(height)+.05f;spriteCoords[7]=(height)/(height);*/

        interfacePull = new interfaceImage(context,spriteCoords);

        textSystem = new textclass(context, "A");
        textList.add(new toText(-.85f,.25f,0,"UCSD Distributed Robotics Lab",0,1));
        textList.add(new toText(-.85f,1f,0,"Scale: "+scale+"x",0,1));
        ardroneTextBegin=2;
        ardroneTextEnd=ardroneTextBegin;
        textList.add(new toText(0,-(width-115)/(height*2)-.05f,0,"No Robots Selected",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline,-(width-115)/(height*2)-.05f,0,"Battery: "+scale+"x",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*2,-(width-115)/(height*2)-.05f,0," X:",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*2,-(width-115)/(height*2)-.05f+tab,0," Y:",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*2,-(width-115)/(height*2)-.05f+tab*2,0," Z:",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*3,-(width-115)/(height*2)-.05f,0,"Vx:",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*3,-(width-115)/(height*2)-.05f+tab,0,"Vy:",1,1));
        ardroneTextEnd++;
        textList.add(new toText(newline*3,-(width-115)/(height*2)-.05f+tab*2,0,"Vz:",1,1));
        ardroneTextEnd++;








        sTemp[0]=-(width-100)/height;sTemp[1]=(height-5)/height;
        sTemp[3]=-(width-100)/height;sTemp[4]=-(height-10)/(height*2);
        sTemp[6]=(width-100)/height;;sTemp[7]=-(height-10)/(height*2);
        sTemp[9]=(width-100)/height;sTemp[10]=(height-5)/height;
        myGrid = new grid(context,sTemp);

    }

    public void setVoronoiCoordinates(float s[],int i,int j){
        vLine[i].setSquareCoords(s);
        vSize=j;
    }

    public void setFreeDrawCoordinates(float s[],int i, int j){
        fLine[i].setSquareCoords(s);
        fSize = j;
    }


    public void setPosition(float f[]) {
        poseData = f;
    }

    @Override
    public void onDrawFrame(GL10 unused) {

        float[] scratch = new float[16];
        // Draw background color
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT | GLES20.GL_DEPTH_BUFFER_BIT);
        // Set the camera position (View matrix)
        Matrix.setLookAtM(mViewMatrix, 0, 0, 0, -3, 0f, 0f, 0f, 0f, 1.0f, 0.0f);
        // Calculate the projection and view transformation
        Matrix.multiplyMM(mMVPMatrix, 0, mProjectionMatrix, 0, mViewMatrix, 0);
        // DRAW OUTLINE
        //mArena.draw(mMVPMatrix);
        //mArena2.draw(mMVPMatrix);
        Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
        Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
        //Matrix.scaleM(scratch,0,scale,scale,scale);
        Matrix.translateM(scratch, 0, 0, 0, 0);
        myGrid.Draw(scratch);
        Matrix.translateM(scratch, 0, 2f, 0, 0);
        myGrid.Draw(scratch);
        Matrix.translateM(scratch, 0, -4f, 0, 0);
        myGrid.Draw(scratch);
        Origin.Draw(mMVPMatrix);


        // DRAW TURTLES
        for (int i=0;i<10;i++){
            if (turtleList[i].getOn()==1) {
                Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
                Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
                Matrix.translateM(scratch, 0, turtleList[i].getX() * scale, turtleList[i].getY() * scale, 0);
                Matrix.rotateM(scratch, 0, turtleList[i].getRot(), 0, 0, 1f);
                Matrix.scaleM(scratch, 0, scale, scale, scale);
                if (turtleList[i].getType()==0){
                    myAr.Draw(scratch,turtleList[i].getState(),framecounter);
                }
                else{
                    turt1.Draw(scratch,turtleList[i].getState());
                }

            }
        }

        Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
        Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
        Matrix.scaleM(scratch, 0, scale, scale, scale);
        // DRAW VORONOI LINES
        if (vToggle==1) {
            for (int i = 0; i < vSize; i++) {
                vLine[i].draw(scratch);
            }
        }

        // DRAW FREE LINES
        if (fToggle==1) {
            for (int i = 0; i < fSize  ; i++) {
                fLine[i].draw(mMVPMatrix);
            }
        }

        // DRAW WAYPOINT
        if (pToggle==1 && pToggle2==1) {
            Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
            Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
            Matrix.translateM(scratch, 0, pX, pY, 0);
            wp.Draw(scratch);
        }

        /*scratch = new float[16];
        Matrix.setRotateM(mRotationMatrix, 0, mAngle, 0, 0, 1.0f);
        Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
        mTriangle.draw(scratch);*/

        // DRAW TARGET MARK
        if (tToggle==1){
            Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
            Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
            Matrix.translateM(scratch, 0, tempX, tempY, 0);
            tar.Draw(scratch);
        }


        //START DRAWING TEXT BLOCK
        //

        Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
        Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
        Matrix.translateM(scratch, 0, -slider, 0, 0);
        interfacePull.Draw(scratch);


        // DRAW VORONOI TOGGLE ICON

        vorToggle.Draw(scratch, vToggle);
        freeDrawToggle.Draw(scratch, fToggle);
        wayPointToggle.Draw(scratch,pToggle);
        ardronePrefToggle.Draw(scratch, APToggle);
        //exit.Draw(scratch,1);
        Matrix.setRotateM(mRotationMatrix, 0, 0, 0, 0, 1.0f);
        Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
        Matrix.translateM(scratch, 0, .75f, -.85f, 0);
        // DRAW BUTTONS
        plus.Draw(scratch,1);


        int temp = 0;
        for (int j = 0; j<textList.size();j++){
            if (textList.get(j).getActive()==1){
                if (APToggle == 1 ||  j < ardroneTextBegin || j > ardroneTextEnd){
                    Matrix.multiplyMM(scratch, 0, mMVPMatrix, 0, mRotationMatrix, 0);
                    if (textList.get(j).getSlides()==1){
                        Matrix.translateM(scratch, 0, textList.get(j).getyGl()-slider, textList.get(j).getxGl(), 0);
                    }
                    else{
                        Matrix.translateM(scratch, 0, textList.get(j).getyGl(), textList.get(j).getxGl(), 0);
                    }

                    for (int i = 0; i<textList.get(j).getText().length();i++){
                        String s = String.valueOf(textList.get(j).getText().charAt(i));
                        if (Character.isUpperCase(textList.get(j).getText().codePointAt(i))==true || s.equals(" ")){
                            if (temp!=0){
                                Matrix.translateM(scratch, 0, -.01f, 0f, 0);
                            }
                            textSystem.Draw(scratch, s, 0);
                            temp++;
                            Matrix.translateM(scratch, 0, -.001f, 0f, 0);
                        }
                        else{
                            textSystem.Draw(scratch, s, 1);
                            temp = 0;
                        }
                        if (s.equals("i")|| s.equals("t")||s.equals("l")|| s.equals("r")){
                            Matrix.translateM(scratch, 0, -.013f, 0f, 0);
                        }
                        else Matrix.translateM(scratch, 0, -.023f, 0f, 0);
                        if (s.equals(".")||s.equals(":") || s.equals(" ")){
                            Matrix.translateM(scratch, 0, .009f, 0f, 0);
                        }
                    }
                }

            }
        }
        //
        //END DRAWING TEXT BLOCK

        framecounter+=1;
        if (framecounter>11){
            framecounter=0;
        }
    }

    public void tempFun(float xx, float yy){
        tempX  =xx;
        tempY  =yy;
    }

    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
        // Adjust the viewport based on geometry changes,
        // such as screen rotation
        GLES20.glViewport(0, 0, width, height);

        float ratio = (float) width / height;

        // this projection matrix is applied to object coordinates
        // in the onDrawFrame() method
        Matrix.frustumM(mProjectionMatrix, 0, -ratio, ratio, -1, 1, 3, 7);

    }

    /**
     * Utility method for compiling a OpenGL shader.
     *
     * <p><strong>Note:</strong> When developing shaders, use the checkGlError()
     * method to debug shader coding errors.</p>
     *
     * @param type - Vertex or fragment shader type.
     * @param shaderCode - String containing the shader code.
     * @return - Returns an id for the shader.
     */
    public static int loadShader(int type, String shaderCode){

        // create a vertex shader type (GLES20.GL_VERTEX_SHADER)
        // or a fragment shader type (GLES20.GL_FRAGMENT_SHADER)
        int shader = GLES20.glCreateShader(type);

        // add the source code to the shader and compile it
        GLES20.glShaderSource(shader, shaderCode);
        GLES20.glCompileShader(shader);

        return shader;
    }

    /**
     * Utility method for debugging OpenGL calls. Provide the name of the call
     * just after making it:
     *
     * <pre>
     * mColorHandle = GLES20.glGetUniformLocation(mProgram, "vColor");
     * MyGLRenderer.checkGlError("glGetUniformLocation");</pre>
     *
     * If the operation is not successful, the check throws an error.
     *
     * @param glOperation - Name of the OpenGL call to check.
     */
    public static void checkGlError(String glOperation) {
        int error;
        while ((error = GLES20.glGetError()) != GLES20.GL_NO_ERROR) {
            Log.e(TAG, glOperation + ": glError " + error);
            throw new RuntimeException(glOperation + ": glError " + error);
        }
    }

    /**
     * Returns the rotation angle of the triangle shape (mTriangle).
     *
     * @return - A float representing the rotation angle.
     */
    public float getAngle() {
        return mAngle;
    }

    /**
     * Sets the rotation angle of the triangle shape (mTriangle).
     */
    public void setAngle(float angle) {
        mAngle = angle;
    }
    public void setvToggle(int i){
        vToggle=i;
    }
    public int getvToggle(){
        return vToggle;
    }
    public void setfToggle(int i){
        fToggle=i;
    }
    public int getfToggle(){
        return fToggle;
    }
    public void setpToggle(int i){
        pToggle=i;
    }
    public void setpToggle2(int i){
        pToggle2=i;
    }
    public int getpToggle(){
        return pToggle;
    }
    public void setAPToggle(int i){
        APToggle=i;
    }
    public int getAPToggle(){
        return APToggle;
    }
    public void eraseFreeLine(){
        fSize=0;
    }
    public void setWayPointValues(float px, float py){
        pX=px;
        pY=py;
    }
    public float[] toGlobal(float px, float py){
        float[] globalCoord={px*scale, py*scale};
        return globalCoord;
    }
    public float[] toGL(float px, float py){
        float[] glCoord={px/scale, py/scale};
        return glCoord;
    }
    public void setScale(float s){
        scale=s;
    }


}