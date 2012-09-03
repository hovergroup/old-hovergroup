function printMatrix(fid,Mat)

for i = 1:size(Mat,2);
    fprintf(fid,[repmat('%f ',1,size(Mat,1)),'\n'],Mat(i,:));
end