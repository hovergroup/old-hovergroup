% generateTracklinesMPC
% for use with configureKayakMPC
% make some waypoints, compute desired heading (compass bearing), and print
% output to a txt file (for read-in on MOOS side)

% BR, 8/13/2012

% list of waypoints to hit:
% time, desired heading, x1, y1, x2, y2

ifPlot=1;
tvec = linspace(dt,Nsec,(Nsec)/dt);

%tracklineType = 'straight';
%tracklineType = 'hexagon';
%tracklineType = 'pavilion_1turn';

switch tracklineType
    
    case 'straight'
        
        % angle of straight line:
        
        % ang = 90-37;
        
        % straight line:
        
        x = zeros(size(tvec));
        y = zeros(size(tvec));
        desBearing = 180*(size(tvec));
        
%         fid = fopen('straightPts.txt','w');
%         for i = 1:2
%             fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
%                 tvec(i*(secPerLeg/dt)),desBearing(i*dt),x(i),y(i),x(i+1),y(i+1));
%         end
%         fclose(fid);
        

    case 'pavilion_1turn'
        
        %numLegs = 2;
        %secPerLeg = 60
        len = 100;
        ox = 0;
        oy = -30;
        pavAng = deg2rad(37);
        
        x = [0 len*cos(pavAng) len*cos(pavAng)+len];
        y = [0 len*sin(pavAng) len*sin(pavAng)];
        
        if(ifPlot)
            plot(x,y)
            axis equal
        end
        
        %rotate: (already done here...)
        %ang=pavAng; %rad
        % ang = 0; 
        % slight offset:
        ang = deg2rad(-20);
        r = [cos(ang),-sin(ang);sin(ang),cos(ang)];
        
        pts = r*[x;y];
        x = pts(1,:);
        y = pts(2,:);
        
        x = x+ox;
        y = y+oy;
        
        if(ifPlot)
            hold on
            plot(x,y)
        end
        
        desBearing = zeros(1,length(tvec));
        for i = 1:(length(tvec)-1)
            step = ceil((i)/(secPerLeg/dt));

            hDes = atan2((y(step+1)-y(step)),(x(step+1)-x(step)));
            hDes = rad2deg(hDes);
            bearing = 90 - hDes;
            if(bearing<0)
                bearing = bearing+360;
            end
            if(bearing>360)
                bearing = bearing - 360;
            end
            desBearing(i) = bearing;

        end
        desBearing(i+1)=bearing;
        
        fid = fopen('pavilionPts.txt','w');
        for i = 1:(numLegs)
            fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
                tvec(ceil(i*(secPerLeg/dt))),desBearing(ceil(i*(secPerLeg/dt))),x(i),y(i),x(i+1),y(i+1));
        end
        
        
        

    case 'hexagon'
        
        % hexagon
        %( grab these from config)
        % dt = 6;
        % numLegs=6;
        % secPerLeg=60;
        % Nsec = secPerLeg*numLegs-dt;
        
        
        
        
        len = 50;
        ox = 0;
        oy = -50;
        
        x = [0 len len+len/2 len 0 -len/2 0];
        y = [0 0 -len*sqrt(3)/2 -len*sqrt(3) -len*sqrt(3) -len*sqrt(3)/2 0];
        
        if(ifPlot)
            plot(x,y)
            axis equal
        end
        
        %rotate:
        ang=.32; %rad
        r = [cos(ang),-sin(ang);sin(ang),cos(ang)];
        
        pts = r*[x;y];
        x = pts(1,:);
        y = pts(2,:);
        
        x = x+ox;
        y = y+oy;
        
        if(ifPlot)
            hold on
            plot(x,y)
        end
        
        desBearing = zeros(1,length(tvec));
        for i = 1:(length(tvec)-1)
            
            step = floor((i)/(secPerLeg/dt))+1;
            hDes = atan2((y(step+1)-y(step)),(x(step+1)-x(step)));
            hDes = rad2deg(hDes);
            bearing = 90 - hDes;
            if(bearing<0)
                bearing = bearing+360;
            end
            if(bearing>360)
                bearing = bearing - 360;
            end
            desBearing(i) = bearing;
            %fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
            %    tvec(i),bearing,x(step),y(step),x(step+1),y(step+1));
            
        end
        desBearing(i+1)=bearing;
        
        fid = fopen('hexagonPts.txt','w');
        for i = 1:(numLegs)
            fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
                tvec(i*(secPerLeg/dt)),desBearing(i*dt),x(i),y(i),x(i+1),y(i+1));
        end
        
        fclose(fid);
end

