package com.example.aaron.test;

import org.ros.concurrent.CancellableLoop;
import org.ros.message.MessageDeclaration;
import org.ros.message.Time;
import org.ros.namespace.GraphName;
import org.ros.node.AbstractNodeMain;
import org.ros.node.ConnectedNode;
import org.ros.node.Node;
import org.ros.node.topic.Publisher;

import java.util.ArrayList;
import java.util.List;

import geometry_msgs.Pose;
import geometry_msgs.PoseArray;

/**
 * Created by sumegamandadi on 8/12/15.
 */
public class messager extends AbstractNodeMain {

    public std_msgs.Int32MultiArray IMA;
    public boolean active=false;
    private int maxNum=50;
    private int intArray[]= new int[50];
    public boolean sent=false;
    private int method=0;
    public String str = new String();

    public messager(){
       for(int i=0;i<maxNum;i++){
           intArray[i]=1;
       }
        str="NOT_INITIALIZED";
    }

    public void setMessage(String s){
        str=s;
    }

    public void setIntArray(int[] intA){
        intArray=intA;
    }

    public void setMethod(int j){
        method = j;
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("messager");
    }



    public void pub(final ConnectedNode connectedNode){
        final Publisher<std_msgs.Int32MultiArray> publisher =
                connectedNode.newPublisher("/message", IMA._TYPE);
        IMA = publisher.newMessage();
        IMA.setData(intArray);
        IMA.getLayout().setDataOffset(0);
        publisher.publish(IMA);
        active=false;
        sent=true;
    }


    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<std_msgs.Int32MultiArray> publisher =
                connectedNode.newPublisher("/message", IMA._TYPE);
        IMA = publisher.newMessage();
        IMA.setData(intArray);
        IMA.getLayout().setDataOffset(0);
        //std_msgs.MultiArrayDimension f = connectedNode.getTopicMessageFactory().newFromType(std_msgs.MultiArrayDimension._TYPE);
/*        IMA.getLayout().getDim().add(f);
        IMA.getLayout().getDim().get(0).setSize(0);
        IMA.getLayout().getDim().get(0).setLabel(str);
        IMA.getLayout().getDim().get(0).setStride(0);*/



        connectedNode.executeCancellableLoop(new CancellableLoop() {

            @Override
            protected void setup() {
            }

            @Override
            protected void loop() throws InterruptedException {
                IMA.setData(intArray);
                IMA.getLayout().setDataOffset(method);
                //IMA.getLayout().getDim().get(0).setLabel(str);
                publisher.publish(IMA);
                active=false;
                sent=true;

            }
        });
    }
}






