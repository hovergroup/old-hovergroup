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

endlat = 42.353153;
endlon = -71.088645;

scalelat = 42.356911;
scalelon2 = -71.088946; %200m
scalelon1 = -71.091435;

figure;
imshow(flipdim(img,1));
hold on

scalex1 = 1000*deg2km(scalelon1-pavlon)/xmperpix+pavx;
scalex2 = 1000*deg2km(scalelon2-pavlon)/xmperpix+pavx;
scaley = 1000*deg2km(scalelat-pavlat)/ymperpix+pavy;
line([scalex1 scalex2],[scaley scaley]);

endx = 1000*deg2km(endlon-pavlon)/xmperpix;
endy = 1000*deg2km(endlat-pavlat)/ymperpix;

line([pavx,endx+pavx],[pavy,endy+pavy]);

x(1) = endx/2-endx/4;
x(2) = endx/2+endx/4;
midx = endx/2;
x(3) = x(1)+(abs(midx-x(1)))*3;
x(4) = midx-(abs(midx-x(1)))*4;

y(1) = endy/2-endy/4;
y(2) = endy/2+endy/4;
midy = endy/2; 
y(3) = y(2)+(abs(midy-y(1)));
y(4) = y(2)+(abs(midy-y(1)));

x = x+pavx;
y = y+pavy;

plot(pavx,pavy,'ro');
plot(endx+pavx,endy+pavy,'ro');

line([x(1),x(3)],[y(1),y(3)],'Color','r');
line([x(2),x(3)],[y(2),y(3)],'Color','g');
line([x(1),x(4)],[y(1),y(4)],'Color','c');
line([x(2),x(4)],[y(2),y(4)],'Color','y');

n=10;

rx = linspace(x(1),x(3),n);
ry = linspace(y(1),y(3),n);

gx = linspace(x(1),x(4),n);
gy = linspace(y(1),y(4),n);

cx = linspace(x(2),x(3),n);
cy = linspace(y(2),y(3),n);

yx = linspace(x(2),x(4),n);
yy = linspace(y(2),y(4),n);

%scatter(rx,ry);
%scatter(gx,gy);
%scatter(cx,cy);
%scatter(yx,yy);

for i=1:n
    xpts{i} = linspace(rx(i),yx(n+1-i),n);
    ypts{i} = linspace(ry(i),yy(n+1-i),n);
    scatter(xpts{i},ypts{i});
end

set(gca,'ydir','normal');

fileID = fopen('relay_waypoints.txt','w');
for i=1:n
    myx = xpts{i};
    myy = ypts{i};   
    
    if(mod(i,2)==0)
       myx = fliplr(myx);
       myy = fliplr(myy);
    end
    
    for j=1:n
        mylon = km2deg((myx(j)-pavx)*xmperpix/1000) + pavlon;
        mylat = km2deg((myy(j)-pavy)*ymperpix/1000) + pavlat;
        fprintf(fileID,'%f,%f\n',mylat,mylon);
    end
end
fclose(fileID);