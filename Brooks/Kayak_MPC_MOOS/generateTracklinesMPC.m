% generateTracklinesMPC
% for use with configureKayakMPC
% make some waypoints, compute desired heading (compass bearing), and print
% output to a txt file (for read-in on MOOS side)
% also outputs desBearing - timestep-indexed vector of des Bearing in deg.

% BR, 8/13/2012

% changes:
%{
- 8/15/2012 - moved major settings to configureKayakMPC
- 8/20/2012 - got rid of step var (conflict with step response)
- 8/29/2012 - changed a bit for heading inputs, conversions to angles,
    - also added copy file to waypoints.txt on non PC systems

%}


% list of waypoints to hit:
% time, desired heading, x1, y1, x2, y2

% pavilion dock angle rel. to horizontal (in True N frame):
ifPlot=0;

MOOSpathName2 = '/home/brooks/hovergroup/ivp-extend/brooks/missions/';
MOOSpathName1 = '/home/josh/hovergroup/ivp-extend/brooks/missions/';

tvec = linspace(dt,Nsec,ceil((Nsec)/dt));

switch tracklineType
    
    case 'straight'
        
        % straight line:
        desB = deg2rad(startHeading);
        x = speed*dt*(0:length(tvec)-1);
        y = zeros(size(tvec));
        if(ifPlot)
            plot(x,y);
            axis equal
        end
        desBearing = rad2deg(desB)*ones(size(tvec));
        % rotation angle is not a bearing
        ang = pi/2 - desB;
        r = [cos(ang),-sin(ang);sin(ang),cos(ang)];
        pts = r*[x;y];
        x = pts(1,:);
        y = pts(2,:);
        x = x+ox;
        y = y+oy;
        
        if(ifPlot)
            hold on
            plot(x,y,'r')
        end
        
        fid = fopen('straightPts.txt','w');
        for i = 1
            fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
                tvec(end),desBearing(end),x(1),y(1),x(end),y(end));
        end
        fclose(fid);
        
        if(~ispc)
            try
                dest = [MOOSpathName1 'waypoints.txt'];
                copyfile('straightPts.txt',dest,'f')
                disp('writing to josh')
            end
            try
                dest = [MOOSpathName2 'waypoints.txt'];
                copyfile('straightPts.txt',dest,'f')
                disp('writing to brooks')
            end
        end
        
        
    case 'oneturn'
        
        len = speed*secPerLeg;
        
        x = [0 len len+len*cos(deg2rad(kinkAng))];
        y = [0 0 -len*sin(deg2rad(kinkAng))];
        
        if(ifPlot)
            plot(x,y)
            axis equal
        end
        
        startAngle = 90-startHeading;
        %rotate:
        ang = deg2rad(startAngle);  %pavAng + deg2rad(pavAngOffset);
        r = [cos(ang),-sin(ang);sin(ang),cos(ang)];
        
        pts = r*[x;y];
        x = pts(1,:);
        y = pts(2,:);
        
        x = x+ox;
        y = y+oy;
        
        if(ifPlot)
            hold on
            plot(x,y,'r')
        end
        
        desBearing = zeros(1,length(tvec));
        for i = 1:(length(tvec)-1)
            leg = ceil((i)/(secPerLeg/dt));
            
            hDes = atan2((y(leg+1)-y(leg)),(x(leg+1)-x(leg)));
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
        
        fid = fopen('oneturnPts.txt','w');
        for i = 1:(numLegs)
            fprintf(fid,'%f,%f,%f,%f,%f,%f\n',...
                tvec(ceil(i*(secPerLeg/dt))),desBearing(ceil(i*(secPerLeg/dt))),x(i),y(i),x(i+1),y(i+1));
        end
        fclose(fid);
        
        if(~ispc)
            try
                dest = [MOOSpathName1 'waypoints.txt'];
                copyfile('oneturnPts.txt',dest,'f')
                disp('writing to josh')
            end
            try
                dest = [MOOSpathName2 'waypoints.txt'];
                copyfile('oneturnPts.txt',dest,'f')
                disp('writing to brooks')
            end
        end
        
    case 'hexagon'
        
        len = speed*secPerLeg;
        %         ox = 0;
        %         oy = -50;
        
        x = [0 len len+len/2 len 0 -len/2 0];
        y = [0 0 -len*sqrt(3)/2 -len*sqrt(3) -len*sqrt(3) -len*sqrt(3)/2 0];
        
        if(ifPlot)
            plot(x,y)
            axis equal
        end
        
        %rotate:
        ang=deg2rad(90 - startHeading); %rad
        r = [cos(ang),-sin(ang);sin(ang),cos(ang)];
        
        pts = r*[x;y];
        x = pts(1,:);
        y = pts(2,:);
        
        x = x+ox;
        y = y+oy;
        
        if(ifPlot)
            hold on
            plot(x,y,'r')
        end
        
        desBearing = zeros(1,length(tvec));
        for i = 1:(length(tvec)-1)
            
            leg = floor((i)/(secPerLeg/dt))+1;
            hDes = atan2((y(leg+1)-y(leg)),(x(leg+1)-x(leg)));
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
                tvec(ceil(i*(secPerLeg/dt))),desBearing(ceil(i*dt)),x(i),y(i),x(i+1),y(i+1));
        end
        
        fclose(fid);
        
        if(~ispc)
            dest = [MOOSpathName 'waypoints.txt'];
            copyfile('hexagonPts.txt',dest,'f')
        end
        
end

