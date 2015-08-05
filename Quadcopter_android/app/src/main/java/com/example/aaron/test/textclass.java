package com.example.aaron.test;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES20;
import android.opengl.GLUtils;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;

import javax.microedition.khronos.opengles.GL10;

import std_msgs.Char;

/**
 * Created by aaron on 6/16/15.
 */public class textclass
{
    //Reference to Activity Context
    private final Context mActivityContext;

    //Added for Textures
    private final FloatBuffer mCubeTextureCoordinates;
    private int mTextureUniformHandle;
    private int mTextureCoordinateHandle;
    private final int mTextureCoordinateDataSize = 2;
    private int mTextureDataHandle, mTextureDataHandleundercase;


    private final String vertexShaderCode =
//Test
            "attribute vec2 a_TexCoordinate;" +
                    "varying vec2 v_TexCoordinate;" +
//End Test
                    "uniform mat4 uMVPMatrix;" +
                    "attribute vec4 vPosition;" +
                    "void main() {" +
                    "  gl_Position =  uMVPMatrix*vPosition;" +
                    //Test
                    "v_TexCoordinate = a_TexCoordinate;" +
                    //End Test
                    "}";

    private final String fragmentShaderCode =
            "precision mediump float;" +
                    "uniform vec4 vColor;" +
//Test
                    "uniform sampler2D u_Texture;" +
                    "varying vec2 v_TexCoordinate;" +
//End Test
                    "void main() {" +
//"gl_FragColor = vColor;" +
                    "gl_FragColor = (vColor * texture2D(u_Texture, v_TexCoordinate));" +
                    "}";

    private final int shaderProgram;
    private final FloatBuffer vertexBuffer;
    private final ShortBuffer drawListBuffer;
    private int mPositionHandle;
    private int mColorHandle;
    private int mMVPMatrixHandle;

    // number of coordinates per vertex in this array
    static final int COORDS_PER_VERTEX = 2;
    static float spriteCoords[] = {
            -0.02f,  0.02f,   // top left
            -0.02f, -0.02f,   // bottom left
            0.02f, -0.02f,   // bottom right
            0.02f,  0.02f}; //top right

    private short drawOrder[] = { 0, 1, 2, 0, 2, 3 }; //Order to draw vertices
    private final int vertexStride = COORDS_PER_VERTEX * 4; //Bytes per vertex

    // Set color with red, green, blue and alpha (opacity) values
    float color[] = { 255f, 255f, 255f, 1.0f };

    public textclass(final Context activityContext, String string)
    {
        mActivityContext = activityContext;

        //Initialize Vertex Byte Buffer for Shape Coordinates / # of coordinate values * 4 bytes per float
        ByteBuffer bb = ByteBuffer.allocateDirect(spriteCoords.length * 4);
        //Use the Device's Native Byte Order
        bb.order(ByteOrder.nativeOrder());
        //Create a floating point buffer from the ByteBuffer
        vertexBuffer = bb.asFloatBuffer();
        //Add the coordinates to the FloatBuffer
        vertexBuffer.put(spriteCoords);
        //Set the Buffer to Read the first coordinate
        vertexBuffer.position(0);

        // S, T (or X, Y)
        // Texture coordinate data.
        // Because images have a Y axis pointing downward (values increase as you move down the image) while
        // OpenGL has a Y axis pointing upward, we adjust for that here by flipping the Y axis.
        // What's more is that the texture coordinates are the same for every face.
        /*final float[] cubeTextureCoordinateData =
                {
                        0f,  1f,
                        .05f, 1f,
                        .05f, 0f,
                        0f, 0f

                };*/
            /*0f,  1f,
            1f, 1f,
            1f, 0f,
            0f, 0f*/

        final float[] cubeTextureCoordinateData = new float[328];

        for (int i =0;i<cubeTextureCoordinateData.length;i++){
            if (i%8==1) {
                cubeTextureCoordinateData[i] =(float)(Math.floor(i/8)/42.78f);
            }
            else if(i%8==0) {
                cubeTextureCoordinateData[i] = 1f;
            }
            else if (i%8==3) {
                cubeTextureCoordinateData[i] = (float)Math.floor((i+5)/8)/42.78f-.005f;
            }
            else if(i%8==2) {
                cubeTextureCoordinateData[i] = 1f;
            }
            else if(i%8==5) {
                cubeTextureCoordinateData[i] = (float)Math.floor((i+5)/8)/42.78f-.005f;
            }
            else if(i%8==4) {
                cubeTextureCoordinateData[i] = 0f;
            }
            else if(i%8==7) {
                cubeTextureCoordinateData[i] = (float)Math.floor(i/8)/42.78f;
            }
            else if(i%8==6) {
                cubeTextureCoordinateData[i] = 0f;
            }

        }



        mCubeTextureCoordinates = ByteBuffer.allocateDirect(cubeTextureCoordinateData.length * 4).order(ByteOrder.nativeOrder()).asFloatBuffer();
        mCubeTextureCoordinates.put(cubeTextureCoordinateData).position(0);

        //Initialize byte buffer for the draw list
        ByteBuffer dlb = ByteBuffer.allocateDirect(spriteCoords.length * 2);
        dlb.order(ByteOrder.nativeOrder());
        drawListBuffer = dlb.asShortBuffer();
        drawListBuffer.put(drawOrder);
        drawListBuffer.position(0);

        int vertexShader = MyGLRenderer.loadShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
        int fragmentShader = MyGLRenderer.loadShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

        shaderProgram = GLES20.glCreateProgram();
        GLES20.glAttachShader(shaderProgram, vertexShader);
        GLES20.glAttachShader(shaderProgram, fragmentShader);

        //Texture Code
        GLES20.glBindAttribLocation(shaderProgram, 0, "a_TexCoordinate");

        GLES20.glLinkProgram(shaderProgram);

        //Load the texture
        mTextureDataHandle = loadTexture(mActivityContext, R.drawable.textsprites2);
        mTextureDataHandleundercase = loadTexture(mActivityContext, R.drawable.textspritesunder2);
    }

    public void Draw(float[] mvpMatrix, String s, int k)
    {
        int j=52;
        if (s.equals("A")|| s.equals("a")){
            j=0;
        }
        else if (s.equals("B")||s.equals("b")){
            j=8;
        }
        else if (s.equals("C")||s.equals("c")){
            j=16;
        }
        else if (s.equals("D")||s.equals("d")){
            j=24;
        }
        else if (s.equals("E")||s.equals("e")){
            j=32;
        }
        else if (s.equals("F")||s.equals("f")){
            j=40;
        }
        else if (s.equals("G")||s.equals("g")){
            j=48;
        }
        else if (s.equals("H")||s.equals("h")){
            j=56;
        }
        else if (s.equals("I")||s.equals("i")){
            j=64;
        }
        else if (s.equals("J")||s.equals("j")){
            j=72;
        }
        else if (s.equals("K")||s.equals("k")){
            j=80;
        }
        else if (s.equals("L")||s.equals("l")){
            j=88;
        }
        else if (s.equals("M")||s.equals("m")){
            j=96;
        }
        else if (s.equals("N")||s.equals("n")){
            j=104;
        }
        else if (s.equals("O")||s.equals("o")){
            j=112;
        }
        else if (s.equals("P")||s.equals("p")){
            j=120;
        }
        else if (s.equals("Q")||s.equals("q")){
            j=128;
        }
        else if (s.equals("R")||s.equals("r")){
            j=136;
        }
        else if (s.equals("S")||s.equals("s")){
            j=144;
        }
        else if (s.equals("T")||s.equals("t")){
            j=152;
        }
        else if (s.equals("U")||s.equals("u")){
            j=160;
        }
        else if (s.equals("V")||s.equals("v")){
            j=168;
        }
        else if (s.equals("W")||s.equals("w")){
            j=176;
        }
        else if (s.equals("X")||s.equals("x")){
            j=184;
        }
        else if (s.equals("Y")||s.equals("y")){
            j=192;
        }
        else if (s.equals("Z")||s.equals("z")){
            j=200;
        }
        else if (s.equals("1")){
            j=208;
        }
        else if (s.equals("2")){
            j=216;
        }
        else if (s.equals("3")){
            j=224;
        }
        else if (s.equals("4")){
            j=232;
        }
        else if (s.equals("5")){
            j=240;
        }
        else if (s.equals("6")){
            j=248;
        }
        else if (s.equals("7")){
            j=256;
        }
        else if (s.equals("8")){
            j=264;
        }
        else if (s.equals("9")){
            j=272;
        }
        else if (s.equals("0")){
            j=280;
        }
        else if (s.equals(" ")){
            j=288;
        }
        else if (s.equals("-")){
            j=296;
        }
        else if (s.equals(":")){
            j=304;
        }
        else if (s.equals(".")){
            j=312;
        }



        GLES20.glUseProgram(shaderProgram);

        //Get handle to vertex shader's vPosition member
        mPositionHandle = GLES20.glGetAttribLocation(shaderProgram, "vPosition");

        //Enable a handle to the triangle vertices
        GLES20.glEnableVertexAttribArray(mPositionHandle);

        //Prepare the triangle coordinate data
        GLES20.glVertexAttribPointer(mPositionHandle, COORDS_PER_VERTEX, GLES20.GL_FLOAT, false, vertexStride, vertexBuffer);

        //Get Handle to Fragment Shader's vColor member
        mColorHandle = GLES20.glGetUniformLocation(shaderProgram, "vColor");

        //Set the Color for drawing the triangle
        GLES20.glUniform4fv(mColorHandle, 1, color, 0);

        //Set Texture Handles and bind Texture
        mTextureUniformHandle = GLES20.glGetAttribLocation(shaderProgram, "u_Texture");
        mTextureCoordinateHandle = GLES20.glGetAttribLocation(shaderProgram, "a_TexCoordinate");

        //Set the active texture unit to texture unit 0.
        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        j=j+8;
        //Bind the texture to this unit.
            if (k==0){
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDataHandle);
            }
        else {
                GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureDataHandleundercase);

            }


        //Tell the texture uniform sampler to use this texture in the shader by binding to texture unit 0.
        GLES20.glUniform1i(mTextureUniformHandle, 0);

        //Pass in the texture coordinate information
        mCubeTextureCoordinates.position(j);
        GLES20.glVertexAttribPointer(mTextureCoordinateHandle, mTextureCoordinateDataSize, GLES20.GL_FLOAT, false, 0, mCubeTextureCoordinates);
        GLES20.glEnableVertexAttribArray(mTextureCoordinateHandle);

        //Get Handle to Shape's Transformation Matrix
        mMVPMatrixHandle = GLES20.glGetUniformLocation(shaderProgram, "uMVPMatrix");

        //Apply the projection and view transformation
        GLES20.glUniformMatrix4fv(mMVPMatrixHandle, 1, false, mvpMatrix, 0);
        GLES20.glBlendFunc(GLES20.GL_SRC_ALPHA, GLES20.GL_ONE_MINUS_SRC_ALPHA);
        GLES20.glEnable(GLES20.GL_BLEND);
        //Draw the triangle
        GLES20.glDrawElements(GLES20.GL_TRIANGLES, drawOrder.length, GLES20.GL_UNSIGNED_SHORT, drawListBuffer);

        //Disable Vertex Array
        GLES20.glDisableVertexAttribArray(mPositionHandle);


    }

    public static int loadTexture(final Context context, final int resourceId)
    {
        final int[] textureHandle = new int[1];

        GLES20.glGenTextures(1, textureHandle, 0);

        if (textureHandle[0] != 0)
        {
            final BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;   // No pre-scaling

            // Read in the resource
            final Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), resourceId, options);

            // Bind to the texture in OpenGL
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureHandle[0]);

            // Set filtering
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);

            // Load the bitmap into the bound texture.
            GLUtils.texImage2D(GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

            // Recycle the bitmap, since its data has been loaded into OpenGL.
            bitmap.recycle();
        }

        if (textureHandle[0] == 0)
        {
            throw new RuntimeException("Error loading texture.");
        }

        return textureHandle[0];
    }
}
