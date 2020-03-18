There are currently 6 images in the trainset, they consist of the first imagesfrom folders s1 - s6 of the att_faces

So far running these commands gets me a distance of zero

-t gianpierre asdf "C:\faces\s7\1.pgm" 

-l gianpierre asdf "C:\faces\s7\2.pgm"

This also returns true to the controller



changing the second command to 
-l gianpierre asdf "C:\faces\s8\1.pgm"

will return false


I am literally checking to see if it is the same "id" if it is true return 1.0, if the id matches one of the training faces return 0.5