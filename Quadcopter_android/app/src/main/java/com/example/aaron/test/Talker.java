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
import org.ros.message.Time;
import org.ros.namespace.GraphName;
import org.ros.node.AbstractNodeMain;
import org.ros.node.ConnectedNode;
import org.ros.node.NodeMain;
import org.ros.node.topic.Publisher;


import geometry_msgs.Point;
import geometry_msgs.Pose;
import std_msgs.Header;

/**
 * A simple {@link Publisher} {@link NodeMain}.
 *
 * @author damonkohler@google.com (Damon Kohler)
 */
public class Talker extends AbstractNodeMain {

    public double num=5;
    public int flag=0;
    private geometry_msgs.Point p;
    public double x,y;
    public geometry_msgs.PoseStamped pose;

    public Talker(double numl){
        x=0;
        y=0;
        num=numl;
    }

    @Override
    public GraphName getDefaultNodeName() {
        return GraphName.of("graphName");
    }


    public void setNum(double numl){
        num=numl;
    }

    public void setPoint(float xx, float yy){
       x=xx; y=yy;
    }

    @Override
    public void onStart(final ConnectedNode connectedNode) {

        final Publisher<geometry_msgs.PoseStamped> publisher =
                connectedNode.newPublisher("/goal_pose", pose._TYPE);

        // This CancellableLoop will be canceled automatically when the node shuts
        // down.
        connectedNode.executeCancellableLoop(new CancellableLoop() {


            @Override
            protected void setup() {
               //num= 0;
            }

            @Override
            protected void loop() throws InterruptedException {

                geometry_msgs.PoseStamped pose = publisher.newMessage();
                pose.getHeader().setFrameId("s");
                pose.getHeader().setStamp(Time.fromMillis(System.currentTimeMillis()));
                pose.getPose().getOrientation().setW(1);
                pose.getPose().getOrientation().setX(0);
                pose.getPose().getOrientation().setY(0);
                pose.getPose().getOrientation().setZ(0);
                pose.getPose().getPosition().setX(x);
                pose.getPose().getPosition().setY(y);
                pose.getPose().getPosition().setZ(0);


                if (flag==1){
                    publisher.publish(pose);

                }
                //Thread.sleep(10000);
            }
        });
    }
}
