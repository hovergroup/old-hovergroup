function printMatrix(fid,Mat)

for i = 1:size(Mat,1);
    fprintf(fid,[repmat('%f ',1,size(Mat,2)),'\n'],Mat(i,:));
end