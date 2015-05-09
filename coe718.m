function y = coe718(X)
 y='const unsigned char ARM[] = {';
 for i=1:230400
     if (mod(i,16)==1)
        y=cat(2,y,sprintf('\n')); 
     end
     y=cat(2,y,'0x');
     for j=1:2
         
        y= cat(2,y,X(i,j));
        
     end
     y=cat(2,y,',');
 end
 y=cat(2,y,':');
end