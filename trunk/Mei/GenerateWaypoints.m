clear all;close all;clc

pavlat = 42.35844;
pavlon = -71.08741;

%Image Properties
img = imread('river.png');
boxsouth = 42.361731;
boxwest = -71.093069;
boxnorth = 42.351836;
boxeast = -71.07262;
pavx = 262; %pixel
pavy = 425; %pixel

xmperpix= abs((1000*deg2km(boxeast-boxwest))/size(img,2));
ymperpix = abs((1000*deg2km(boxnorth-boxsouth))/size(img,1));

endlat = 42.357459;
endlon = -71.078829;

scalelat = 42.356911;
scalelon2 = -71.088946; %200m
scalelon1 = -71.091435;

figure;
imshow(flipdim(img,1));
hold on

scalex1 = 1000*deg2km(scalelon1-pavlon)/xmperpix+pavx;
scalex2 = 1000*deg2km(scalelon2-pavlon)/xmperpix+pavx;
scaley = 1000*deg2km(scalelat-pavlat)/ymperpix+pavy;
%line([scalex1 scalex2],[scaley scaley]);

%endx = 1000*deg2km(endlon-pavlon)/xmperpix;
%endy = 1000*deg2km(endlat-pavlat)/ymperpix;

endx = 700/xmperpix;
endy = -300/ymperpix;

line([pavx,endx+pavx],[pavy,endy+pavy]);

n=3;
total = 150;
fac = 200/n;
x(1) = endx/2-fac;
x(2) = endx/2+fac;
x(3) = endx/2+fac;
x(4) = endx/2-fac;

y(1) = endy/2+fac;
y(2) = endy/2+fac; 
y(3) = endy/2-fac;
y(4) = endy/2-fac;

x = x+pavx;
y = y+pavy;

plot(pavx,pavy,'ro');
plot(endx+pavx,endy+pavy,'ro');

line([x(1),x(2)],[y(1),y(2)],'Color','r');
line([x(2),x(3)],[y(2),y(3)],'Color','g');
line([x(3),x(4)],[y(3),y(4)],'Color','c');
line([x(1),x(4)],[y(1),y(4)],'Color','y');

rx = linspace(x(1),x(2),n);
ry = linspace(y(1),y(2),n);

gx = linspace(x(1),x(4),n);
gy = linspace(y(1),y(4),n);

cx = linspace(x(2),x(3),n);
cy = linspace(y(2),y(3),n);

yx = linspace(x(3),x(4),n);
yy = linspace(y(3),y(4),n);

%scatter(rx,ry);
%scatter(gx,gy);
%scatter(cx,cy);
%scatter(yx,yy);

for i=1:n
    xpts{i} = linspace(rx(i),yx(n+1-i),n);
    xpts{i} = (xpts{i}-pavx)*xmperpix*0.738;
    xpts{i} = xpts{i}/xmperpix+pavx;
    ypts{i} = linspace(ry(i),yy(n+1-i),n);
    scatter(xpts{i},ypts{i},50);
end

hold on

fileID = fopen('relay_waypoints.txt','w');

myx = xpts{1};
myy = ypts{1};       

for j=1:n
        mylon = (myx(j)-pavx)*xmperpix
        mylat = (myy(j)-pavy)*ymperpix
       % scatter(myx(j),myy(j));
        fprintf(fileID,'%g,%g\n',mylon,mylat);
end

for j=2:n
    mylon = (xpts{j}(n)-pavx)*xmperpix;
        mylat = (ypts{j}(n)-pavy)*ymperpix;
       %         scatter(xpts{j}(n),ypts{j}(n));

        fprintf(fileID,'%g,%g\n',mylon,mylat);
end

myx = xpts{n};
myy = ypts{n};       

for j=2:n
        mylon = (myx(n+1-j)-pavx)*xmperpix;
        mylat = (myy(n+1-j)-pavy)*ymperpix;
%scatter(myx(n+1-j),(myy(n+1-j)));

        fprintf(fileID,'%g,%g\n',mylon,mylat);
end

    mylon = (xpts{2}(1)-pavx)*xmperpix;
        mylat = (ypts{2}(1)-pavy)*ymperpix;
        scatter(xpts{2}(1),ypts{2}(1));
        fprintf(fileID,'%g,%g\n',mylon,mylat);
        mylon = (xpts{2}(2)-pavx)*xmperpix;
        mylat = (ypts{2}(2)-pavy)*ymperpix;
        scatter(xpts{2}(2),ypts{2}(2));
        fprintf(fileID,'%g,%g\n',mylon,mylat);

set(gca,'ydir','normal');

fclose(fileID);