// Software    : Scilab
// Function    : varspace()
// Version     : 1.0
// Date        : 2015-07-13
// Author (C)  : Samuel GOUGEON - France
// License     : CeCILL-B : http://www.cecill.info/licences
// Release     : http://fileexchange.scilab.org/toolboxes/364000
// History     :
//   2010-04-01 : Creation
//   2015-07-13 : First release on FileExchange
//
// DISCLAIMER: This macro is provided as is, with NO WARRANTY of any 
//   kind. Use it to your own risks, awareness, and responsability.

function varargout = varspace(varargin)
//
// Memory space taken by specified variables, litterals, or containers components
//
// CALLING SEQUENCES
// varspace              // displays this help
// varspace var1 var2 ...
// vm = varspace("var1", "var2", ...)
// Vm = varspace(VarNames)
// Vm = varspace(obj#1)
// VM = varspace(obj#1, obj#2, ...)
//
// PARAMETERS
// var1, var2,.. : 
//        Words = names of variables that's memory space must be
//        returned or displayed. 
// VarNames : 
//        Vector or matrix of non-empty words = variables names
// vm :   Row of decimal numbers = memory space allocated to associated 
//        variables, in bytes.
//
// Vm :   Vector or matrix of decimal numbers, having the size of 
//        VarNames or obj#1.
//        Vm(i)   = memory allocated to the variable named VarNames(i)
//                   or to the component obj#1(i).
//        Vm(i,j) = memory allocated to the variable named VarNames(i,j)
//                   or to the component obj#1(i,j).
//
// obj#1, obj#2,.. : 
//        Literal objects, or variables of simple data type, or 
//        heterogeneous containers (Lists, Cells arrays, Structures 
//        arrays). If a component C is itself a container, varspace()
//        is not recursively applied to its components: Only its whole
//        varspace is returned at the C position.
//
// VM :   List of vectors or matrices of decimal numbers. 
//        VM(i) has the size of obj#i. It provides the vector or matrix 
//        of numbers of bytes allocated to the components of obj#i: 
//        - literal objects or expressions: the object or the result 
//          of the expression is first assigned to a temporary variable.
//          The memory space taken by this one is returned. 
//        - words or texts (scalar, vector or matrix): are considered 
//          as objects in their own, not as names of variables to be 
//          byte-sized. 
//
// DESCRIPTION
//  For a series of scilab variables, varspace() returns the respective
//   amounts of allocated memory, in bytes.
//  If a queried variable or a component of a list or stucture does 
//   not exist, a memory amount == -1 is returned.
//  NOTE: Only local variables are seen. To make a global variable locally
//   visible, please apply global() to it before calling varspace().
//  NOTE: If two variables with the same name are defined in the local 
//   and the global spaces and are queried, only the local one is 
//   considered.
//
// EXAMPLES
//    clear d
//    a = 1, b = [1 2], c = [1 2 3], e = [], es = '', s1 = 'a', s2 = 'ab'
//    ec = cell(); ec22 = cell(2,2); ec44 = cell(4,4); ec88 = cell(8,8);
//
//// Specifying names of variables 
//// -----------------------------
//    varspace ec ec22 ec44 ec88 // inline syntax (then without coma)
//    varspace('a,b ,c, d, e,es, s1,s2, ec22 ec88, a') 
//    varspace(['a' 'b' 'c' ; 'd' 'ec' 'a']) // matrix of variables names
//
//// Specifying objects
//// ------------------
//// '' can't be the name of a variable 
////  => the matrix of texts is considered as an object:
//    varspace(['a' 'b' 'c' ; 'd' 'e' '']) 
//
//// For containers like a cells array, the memory allocated to each 
//// array's component is returned:
//    c = makecell([2,3], %t, %pi, %z, list('hello', rand(3,5)),'', rand(1,3)<0.5)
//// The memory allocated to the cell is greater than the sum of 
////  memory allocated to its components:
//    varspace(c)
//    sum(varspace(c)), varspace("c")  
//
//// This is also the case for lists, that can have undefined components:
//    m = rand(100,100);
//    L = list(m, m+%i*m, , m<0.5, int8(m), int16(m), int32(m));
//    varspace('m', L)  // Since L is an object, 'm' is considered 
//                      //  as a simple string (another object), 
//                      //  instead of a variable's name
//// To get the varspace of a litteral string, you may embed it in a list:
//   varspace(list("a,b ,c, d, e,es, s1,s2, ec22 ec88, a"))
//// .. or priorly assign it to a variable:
//    s = "a,b ,c, d, e,es, s1,s2, ec22 ec88, a";
//    varspace s
//
// SEE ALSO: 
//  * who : http://help.scilab.org/docs/current/en_US/who.html
//  * http://wiki.scilab.org/Memory%20representation%20of%20variables
  
    if argn(2)==0
        head_comments("varspace")
        varargout = list([])
        return
    end
    
    // TYPE OF INPUT = ?
    objects = %f
    na = lstsize(varargin)
    for i = 1:na
        if type(varargin(i))~=10 | (size(varargin(i),"*")>1 & na>1)
            objects = %t
            break
        end
    end
    if ~objects & find(varargin(1)=="")~=[]
        objects = %t
    end
  
    // INPUT = NAMES OF VARIABLES
    if ~objects 
        if na>1 then
            v = string(list2vec(varargin))
        else
            v = varargin(1)(:)
        end
        v = strsubst(strcat(v,",")," ",",")
        v = stripblanks(tokens(v,","));// BEWARE: Consecutive ,, are skipped
        [Gn, Gs] = who('global');
        [Ln, Ls] = who('local');
        for i=1:size(v,"*")  // Vectorization might be possible
            tmp = find(Ln==v(i));
            if tmp~=[], rep(i) = Ls(tmp(1))
            else
                tmp = find(Gn==v(i));
                if tmp~=[], rep(i) = Gs(tmp(1))
                else rep(i) = -1;
                end
            end
        end
        tmp = rep~=-1;
        rep(tmp) = rep(tmp)*8;// in bytes rather than doubles
        if na==1
            s = size(varargin(1))
            if prod(s)~=size(v,"*")
                rep = rep'
            else
                rep = matrix(rep,s)
            end
        else
            rep = rep'
        end
        varargout = list(rep)
      
  // INPUT = OBJECTS (AND LITERALS)
    else
        in = varargin
        out = list()
        for i = 1:na
            in_i = in(i)
            if typeof(in_i)=="ce"
                in_i = in_i.entries
            end
            if type(in_i)==15    // list
                out_i = []
                for j = 1:lstsize(in_i)
                    tmp = in_i(j)
                    if isdef("tmp","l")
                        [Ln, Ls] = who('local')
                        tmp2 = find(Ln=="tmp");
                        out_i(j) = Ls(tmp2(1))*8
                    else
                        out_i(j) = -1
                    end
                end
                out(i) = out_i'
                if typeof(in(i))=="ce"
                    out(i) = matrix(out_i,double(in(i).dims))
                end
            else
                [Ln, Ls] = who('local')
                tmp2 = find(Ln=="in_i");
                out(i) = Ls(tmp2(1))*8
            end
        end
        if type(out)==15 & lstsize(out)==1
            varargout = out
        else
            varargout = list(out)
        end
    end
endfunction
