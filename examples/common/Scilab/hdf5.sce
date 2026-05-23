// loads all variables from a HDF5 file
function [] = loadHDF5(fileName) 
   a = h5open(fileName);
   allVars = a.root.datasets;
   
   [cols, rows] = size(allVars);
   for i = 1:cols
   		myVar = allVars(i);
   		myRootVar = strsplit(myVar, ".");
		execstr(sprintf('global %s; %s = a(''/%s'').data;", myRootVar(1), myVar, myVar));
	end
	
	h5close(a);
endfunction