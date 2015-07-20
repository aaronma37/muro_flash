%time,field.name,field.pose.position.x,field.pose.position.y,field.pose.position.z,field.pose.orientation.x,field.pose.orientation.y,field.pose.orientation.z,field.pose.orientation.w
clear
clc
count=1;
fclose('all');
fileID = fopen('15.txt');
figure('units','normalized','outerposition',[0 0 1 1])
while 1==1
    
check=fscanf(fileID, '%f,');
if isempty(check)
    break;
end
timeMeasurement(count)=check;
fscanf(fileID, '%[boticelli]');
%  fscanf(fileID, '%f,')
scanner=fscanf(fileID, ',%f');
x(count)=scanner(1);
y(count)=scanner(2);
q1(count)=scanner(6);
q2(count)=scanner(7);
[r1(count) r2(count) r3(count)] = quat2angle([(q2(count)) 0 0 q1(count)]);
r1(count)=r1(count)+3.14;
count=count+1;
end
fclose(fileID);
count=1;
fileID2 = fopen('16.txt');
while 1==1
    
check=fscanf(fileID2, '%f,');
if isempty(check)
    break;
end
timeMeasurement2(count)=check;
fscanf(fileID2, '%[boticelli]');
%  fscanf(fileID, '%f,')
scanner=fscanf(fileID2, ',%f');
x2(count)=scanner(1);
y2(count)=scanner(2);
q12(count)=scanner(6);
q22(count)=scanner(7);

count=count+1;
end
fclose(fileID2);
j=1;
r=100;
xb=350;
yb=250;
ang=0:0.01:2*pi; 
xp=r*cos(ang);
yp=r*sin(ang);
plot(xb+xp,yb+yp);
hold on;
axis equal
title('Kalman Filter Output (Backwards state dynamics)')
axis([150,550,50,450]);
axis square

for i=2:length(timeMeasurement)
    if i==3
        
       legend('Goal Circle','Kalman Filter State Estimate','Measurement','Location','southwest') 
        
    end
   while timeMeasurement2(j)<timeMeasurement(i)
       j=j+1;
   end
   j=j-1;
plot(x(i),y(i),'.')

%      plot(linspace(x(i),x(i)+10*cos(r1(i))),linspace(y(i),y(i)+10*sin(r1(i))))

    plot(x2(j),y2(j),'r.')

    
     
    pause(.001)
% F(i-1)=getframe;
end
