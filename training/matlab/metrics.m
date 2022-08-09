function [precision,recall,f1,accuracy] = metrics(confusion_matrix)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    assert(numel(confusion_matrix) == 9, "confusion_matrix must be 3x3");
    tp0=0; tn0=0; fp0=0; fn0=0;
    tp1=0; tn1=0; fp1=0; fn1=0;
    tp2=0; tn2=0; fp2=0; fn2=0;

    tp0=confusion_matrix(1,1); 
    tn0=confusion_matrix(2,2)+confusion_matrix(2,3)+confusion_matrix(3,2)+confusion_matrix(3,3); 
    fp0=confusion_matrix(2,1)+confusion_matrix(3,1); 
    fn0=confusion_matrix(1,2)+confusion_matrix(1,3);

    tp1=confusion_matrix(2,2); 
    tn1=confusion_matrix(1,1)+confusion_matrix(1,3)+confusion_matrix(3,1)+confusion_matrix(3,3); 
    fp1=confusion_matrix(1,2)+confusion_matrix(2,3); 
    fn1=confusion_matrix(1,2)+confusion_matrix(3,2);

    tp2=confusion_matrix(3,3); 
    tn2=confusion_matrix(1,1)+confusion_matrix(1,2)+confusion_matrix(2,1)+confusion_matrix(2,2); 
    fp2=confusion_matrix(1,3)+confusion_matrix(1,2); 
    fn2=confusion_matrix(3,1)+confusion_matrix(3,2);

    fprintf("Class 0 -> tp: %d tn: %d fp: %d fn: %d\n",tp0,tn0,fp0,fn0);
    fprintf("Class 1 -> tp: %d tn: %d fp: %d fn: %d\n",tp1,tn1,fp1,fn1);
    fprintf("Class 2 -> tp: %d tn: %d fp: %d fn: %d\n",tp2,tn2,fp2,fn2);

    precision = ((tp0/(tp0+fp0))+(tp1/(tp1+fp1))+(tp2/(tp2+fp2)))*(1/3.);
    recall = ((tp0/(tp0+fn0))+(tp1/(tp1+fn1))+(tp2/(tp2+fn2)))*(1/3.);
    accuracy = (((tp0+tn0)/(tp0+tn0+fp0+fn0))+((tp1+tn1)/(tp1+tn1+fp1+fn1))+((tp2+tn2)/(tp2+tn2+fp2+fn2)))*(1/3.);
    f1 = 2*((precision*recall)/(precision+recall)); 


    fprintf("Precision %f\n", precision);
    fprintf("Recall    %f\n", recall);
    fprintf("F1        %f\n", f1);
    fprintf("Accuracy  %f\n", accuracy);

end