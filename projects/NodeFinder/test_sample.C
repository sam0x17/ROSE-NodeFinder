// A Comprehensive test that hits all edge cases
// NodeFinder would have to deal with

void deep_func()
{
   bool a = true; // var 1
   if(a) // if 1
   {
      int target_one; // var 2
      if(a) // if 2
      {
         int target_two; // var 3
         if(a) // if 3
         {
            int target_three; // var 4
            if(a) // if 4
            {
               int target_four; // var 5
               if(a) // if 5
               {
                  int target_five; // var 6
                  int target_six; // var 7
                  if(a) // if 6
                  {
                     if(a) // if 7
                     {
                        for(int i = 0; i < 20; i++) // var 8
                        {
                           for(int i2 = 0; i < 20; i2++) // var 9
                           {
                              for(int i3 = 0; i < 20; i3++) // var 10
                              {
                                 for(int i4 = 0; i < 20; i4++) // var 11
                                 {
                                    if(a); // if 8
                                    if(a); // if 9
                                    if(a); // if 10
                                    if(a); // if 11
                                    bool target_seven; // var 12
                                    int target_eight; // var 13
                                    int target_nine; // var 14
                                 }
                                 int target_ten; // var 15
                                 int target_11; // var 16
                                 if(a); // if 12
                              }
                              if(a); // if 13
                           }
                           if(a); // if 14
                        }
                        if(a); // if 15
                     }
                     if(a); // if 16
                  }
               }
            }
         }
      }
   }
}
