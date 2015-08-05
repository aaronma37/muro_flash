package com.example.aaron.test;

public class turtle {

    public float x=0;
    public float y=0;
    public float z=0;

    public float rot=0;
    public float xOld=0;
    public float yOld=0;
    public float zOld=0;
    public float deltaT=1;
    public float rotOld=0;
    public float vX=0;
    public float vY=0;
    public float vZ=0;
    public float vRot=0;
    public float data[]={0,0,0};
    public float nameID=0;
    public int state=0;
    public int on=0;
    public int type=0;
    public float roll=0;
    public int frequency=50;
    public float pitch=0;
    private String identification;

    public turtle() {
        x=0;y=0;rot=0;z=0;state=0;on=0;type=0; pitch=0;
        xOld=0;
        yOld=0;
        zOld=0;
        rotOld=0;
        vX=0;
        vY=0;
        vZ=0;
        vRot=0;
    }

    public int getState(){return state;}

    public String getIdentification(){
        return identification;
    }

    public void setState(int i){state=i;}

    public float getX() {
        return x;
    }

    public float getY() {
        return y;
    }
    public float getZ(){
        return z;
    }

    public float getRot() {
        return rot;
    }
    public float getPitch(){
        return pitch;
    }
    public float getRoll(){
        return roll;
    }

    public int getOn(){
        return on;
    }

    public void setOn(int s){
        on=s;
    }

    public float[] getData(){
        float data[]={x,y,z,rot,nameID, state,on,pitch,roll};
        return data;
    }


    // Return either turtlebot or ar drone (0 == drone)
    public int getType(){
        return type;
    }

    public void setData(float[] f, String s, int t) {
        vX=(f[0]-xOld)*frequency;
        vY=(f[1]-yOld)*frequency;
        vZ=(f[2]-zOld)*frequency;
        vRot=(f[3]-rotOld)*frequency;
        xOld=f[0];
        yOld=f[1];
        zOld=f[2];
        rotOld=f[3];
        x=f[0];
        y=f[1];
        z=f[2];
        rot=f[3];
        nameID=f[4];
        state=(int)f[5];
        on=(int)f[6];
        pitch= f[7];
        roll= f[8];
        identification=s;
        type=t;
    }

    public float getvX(){
        return vX;
    }
    public float getvY(){
        return vY;
    }
    public float getvz(){
        return vZ;
    }
    public float getvRot(){
        return vRot;
    }

    public void runPredictor(){
        x=x+(float)(.01*vX);
        y=y+(float)(.01*vY);
        z=z+(float)(.01*vZ);
        vX=vX*.75f;
        vZ=vZ*.75f;
        vY=vZ*.75f;
        rot=rot;//+(float)(.01*vRot*.01);
    }
}