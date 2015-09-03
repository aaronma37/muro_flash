package com.example.aaron.test;


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


public class pingListener<T> implements NodeMain {

    public std_msgs.Empty p;
    public boolean received = false;
    public float timeEnd;

    public pingListener() {

    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("poseFinder");
    }

    @Override
    public void onStart(ConnectedNode connectedNode) {
        Subscriber<std_msgs.Empty> subscriber = connectedNode.newSubscriber("/pingBack", p._TYPE);
        subscriber.addMessageListener(new MessageListener<std_msgs.Empty>() {
            @Override
            public void onNewMessage(std_msgs.Empty pose) {

            timeEnd=System.currentTimeMillis();

                System.out.println("Ping End:" + timeEnd);

                received = true;
            }
        }, 1);
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