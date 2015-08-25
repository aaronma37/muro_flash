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


import android.preference.PreferenceActivity;

import org.ros.concurrent.CancellableLoop;
import org.ros.internal.message.DefaultMessageFactory;
import org.ros.message.MessageFactory;
import org.ros.message.Time;
import org.ros.namespace.GraphName;
import org.ros.node.AbstractNodeMain;
import org.ros.node.ConnectedNode;
import org.ros.node.Node;
import org.ros.node.NodeMain;
import org.ros.node.topic.Publisher;


import java.util.Random;

import geometry_msgs.Point;
import geometry_msgs.Pose;
import std_msgs.Header;

/**
 * A simple {@link Publisher} {@link NodeMain}.
 *
 * @author damonkohler@google.com (Damon Kohler)
 */
public class dummyMaker extends AbstractNodeMain {

    public double num=5;
    public int flag=0;
    private geometry_msgs.Point p;
    private MessageFactory messageFactory;
    public double x,y;
    public tf2_msgs.TFMessage pose;
    public geometry_msgs.TransformStamped tf;


    Node node;

    public dummyMaker(double numl){
        x=0;
        y=0;
        num=numl;
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("idk");
    }


    public void setNum(double numl){
        num=numl;
    }

    public void setPoint(float xx, float yy){
        x=xx; y=yy;
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<tf2_msgs.TFMessage> publisher =
                connectedNode.newPublisher("/tf", pose._TYPE);

        final Publisher<geometry_msgs.TransformStamped> publisher2 =
                connectedNode.newPublisher("/tf", tf._TYPE);

        // This CancellableLoop will be canceled automatically when the node shuts
        // down.
        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {
                //num= 0;
            }

            @Override
            protected void loop() throws InterruptedException {


                tf2_msgs.TFMessage pose = publisher.newMessage();

                geometry_msgs.TransformStamped tf = connectedNode.getTopicMessageFactory().newFromType(geometry_msgs.TransformStamped._TYPE);
                tf.getTransform().getTranslation().setX(Math.random() * .1);
                tf.getTransform().getTranslation().setY(Math.random() * .1);
                tf.getTransform().getTranslation().setZ(Math.random() * .1);
                tf.getTransform().getRotation().setX(0);
                tf.getTransform().getRotation().setY(0);
                tf.getTransform().getRotation().setZ(0);
                tf.getTransform().getRotation().setW(1);
                tf.getHeader().setFrameId("ORB_SLAM/World");
                tf.getHeader().setSeq(1);
                tf.setChildFrameId("Dummy");

                pose.getTransforms().add(tf);
                pose.getTransforms().set(0,tf);
                if (flag==1){
                    publisher.publish(pose);
                }
                flag=0;
                //Thread.sleep(10000);
            }
        });
    }
}
