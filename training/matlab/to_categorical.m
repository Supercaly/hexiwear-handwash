function catLabels = to_categorical(labels,num_classes)
    catLabels= zeros(size(labels,1),num_classes);
    for i = 1:size(labels,1)
        catLabels(i,labels(i)+1) = 1;
    end
end