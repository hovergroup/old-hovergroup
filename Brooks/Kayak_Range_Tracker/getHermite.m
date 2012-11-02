% Get abscissas and weights for Hermite (Gaussian) quadrature.
% Notice that the calling argument is a dummy (not used).
% This sets up three dimensions as written; the number of
% quadrature points in each dimension is selected in the
% top few lines.

% FSH MIT MechE October 2012

function [s1,s2,s3,w,vol] = getHermite(~) 

% (first nonzero case will be used)
if 0,
	x = sqrt(2)/2 ; % 2 points will be created
	w = .8862269 ;
elseif 1,
	x = [0 1.2247499] ; % 3 points
	w = [1.1816359 .2954090]; 
elseif 1,
	x = [0 .9585725 2.0201829] ; % 5 points
	w = [.9453087 .3936193 .0199532] ;
elseif 1,
	x = [0 .8162879 1.6735516 2.6519614] ; % 7 points
	w = [.8102646 .4256073 .0545156 .0009718] ;
end;

vol = pi^(3/2); % volume - we'll need this later for scaling

% fill out the whole 1d quadrature points and weights
if length(x) > 1,
	x = [fliplr(-x(2:end)) x] ;
	w = [fliplr(w(2:end)) w] ;
else
	x = [-x x] ;
	w = [w w] ;
end;

% normalize so that the 1d quadrature has unit variance
x = x*sqrt(2) / sqrt(length(x));
disp(sprintf('Variance of basic x:  %g (1).', var(x)));

% normalize again so that the covariance matrix is the identity
x = x * sqrt(length(x)) ; 
disp(sprintf('Variance of corrected x:  %g.', var(x)));

% make abscissas to be assigned to unique sigma points
for i = 1:length(x),
	for j = 1:length(x),
		for k = 1:length(x),
			s1(i,j,k) = x(i) ;
			s2(i,j,k) = x(j) ;
			s3(i,j,k) = x(k) ;
		end;
	end;
end;