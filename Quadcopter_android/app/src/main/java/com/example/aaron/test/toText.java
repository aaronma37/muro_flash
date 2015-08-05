package com.example.aaron.test;

/**
 * Created by aaron on 7/7/15.
 */
public class toText {
    private float xGl=0;
    private float yGl=0;
    private int slides=1;
    private int active=0;
    String text;
    int font=0;

    public toText(){
        xGl=0;
        yGl=0;
        font=0;
        text= "empty text>>>";
        active=0;
    }

    public toText(float x, float y, int j, String s, int k, int z){
        slides=k;
        active=z;
        xGl=x;
        yGl=y;
        font=j;
        text=s;
    }

    public float getxGl(){
        return xGl;
    }
    public float getyGl(){
        return yGl;
    }
    public float getActive(){return active;}
    public int getSlides(){
        return slides;
    }
    public int getFont(){
        return font;
    }
    public String getText(){
        return text;
    }

    // SET VALUES

    public void setxGl(float x){
        xGl=x;
    }
    public void setyGl(float y){
        yGl=y;
    }
    public void setFont(int j){
        font=j;
    }
    public void setText(String s){
        text=s;
    }
    public void setSlides(int k){
        slides=k;
    }
    public void setActive(int z){active=z;}
}
