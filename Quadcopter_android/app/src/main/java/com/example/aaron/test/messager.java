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

    public std_msgs.String str;
    public boolean active=false;
    public boolean sent=false;

    public messager(){
        //str.setData("Empty");
    }

    public void setMessage(String s){
        //str.setData(s);
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("messager");
    }





    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<std_msgs.String> publisher =
                connectedNode.newPublisher("/messages", str._TYPE);

        str = publisher.newMessage();

        connectedNode.executeCancellableLoop(new CancellableLoop() {

            @Override
            protected void setup() {
            }

            @Override
            protected void loop() throws InterruptedException {
                publisher.publish(str);
                active=false;
                sent=true;
            }
        });
    }
}






