// Golfscore Version 1.1

// basic file operations
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

const int TRUE = 1;
const int FALSE = 0;
const int GOLFERMAX = 12;                   // There can be at most 12 golfers
const int COURSEMAX = 5;                    // There can be at most 5 courses

int tRep = FALSE;                           // Tournament Ranking Report flag
int gRep = FALSE;                           // Golfer Report flag
int cRep = FALSE;                           // Course Report flag
int golfercount = 0;                        // Counts the number of golfers
int coursecount = 0;                        // Counts the number of courses

int duplicate;                              // Duplicate record flag
char temp[2];                               // Temporary string holder for atoi()
char tempID;                                // Holds Course ID in integer format
char tempName[21];                          // Used to hold golfer's name
char tempPar, tempScore;                    // For par values and hole scores
int courseIndex;                            // Used as an index
int temp1, temp2, rank;
char tempChar;
char tempDir[80];
char tOutfile[80];
char gOutfile[80];
char cOutfile[80];
char answer;                                // For user's answer to queries

int golferSort[GOLFERMAX][2];               // Used for sorting Golfers by rank
char nameSort[GOLFERMAX][21];               // Used for sorting Golfers by name
int indexSort[GOLFERMAX];                   // Also used for sorting Golfers by name
char string1[21];

class Course                                // A golf course
{
      public: char name[19];                // The course's name
      public: char ID;                      // The course's identifier
      public: int par[18];                  // Par values for all 18 holes
};

class Golfer                                // A golfer
{
      public: char name[21];                // Golfer's name
      public: char IDs[COURSEMAX];          // Course ID's for this golfer
      public: int scores[COURSEMAX][20];    // 18 stroke counts or scores (I'm
                                            // not sure which), a hole total,
                                            // and the tournament score for
                                            // each of COURSEMAX courses
      public: int cnt;                      // Keeps track of how many courses
                                            // this golfer has played
      public: int rankScore;                // Final score for this golfer
      public: int Rank;                     // Golfer's rank for the tournament
};

Course * courses[COURSEMAX];                // An array of course pointers
Golfer * golfers[GOLFERMAX];                // An array of golfer pointers

void heaperror(void);

int main (int argc, char * argv[])
{
   const int BUF_SIZE = 80;
   char buffer[BUF_SIZE];
   char out_buff[80];
   ifstream myfile2, fin, tfile, cfile, gfile;
   ofstream myfile, fout;
   cout << "This is GolfScore Rev. 1.1\n";
   time_t now;
   now=time(NULL);
   cout << "Date and time:  " << ctime(&now) << endl;

// ******************************************
// ***	Parsing the call line parameters  ***
// ******************************************
   if (argc < 2)   						 // There has to be at least one option
   {
          cerr << "Usage:  >golf <options> <filename> <output-directory>" << endl;
      return -1;
   }

   if (strlen(argv[1]) < 2) 			 // Only one character in first parameter
   {
          cerr << "ERROR: First command line argument must be from 2 to 4 characters" << endl;
      cerr << "GolfScore will terminate\n";
      return -1;
   }

   if (argv[1][0] != '-')				 // First parameter must begin with a hyphen
   {
          cerr << "ERROR: Options must begin with a '-'" << endl;
      cerr << "GolfScore will terminate\n";
      return -1;
   }

   if (toupper(argv[1][1]) == 'H') 	     // Check for Help mode
   {
          cout << "The call format for GolfScore is as follows:\n";
          cout << ">golf <options> <filename> <output-directory>\n";
      cout << "<options> may be -h or any combination of -c, -g, -t (e.g. -ct, -gTC, -G, etc.)\n";
      cout << "-h displays this help massage.\n-c, -g and -t generate the Course Report,\n";
      cout << "Golfer Report and Tournament Ranking Report respectively.\n";
      cout << "In Help mode, <filename> and <output-directory> are ignored.\n";
      cout << "In Report Generation mode, <filename> contains the tornament data and\n";
      cout << "<output-directory> is the directory where the output files are to be stored.";
      cout << "\n\nEND OF HELP INFO\n";
      return -7;
   }

   tempChar = toupper(argv[1][1]);	    // Convert first option to a capital letter
   if (tempChar == 'C')   			    // Check first character of first parameter
        cRep = TRUE;
   if (tempChar == 'G')
        gRep = TRUE;
   if (tempChar == 'T')
        tRep = TRUE;
   if (tempChar != 'C' && tempChar != 'G' && tempChar != 'T')  // Must be 'c', 'g' or 't'
   {
        cerr << "ERROR: Invalid option: " << argv[1] << endl;
      cerr << "GolfScore will quit now\n";
      return -1;
   }

   if (strlen(argv[1]) > 2)    		     // If there is a second character...
   {
        tempChar = toupper(argv[1][2]);
      if (tempChar == 'C') 		       // ...check second character of first parameter
                cRep = TRUE;
        if (tempChar == 'G')
                gRep = TRUE;
        if (tempChar == 'T')
                tRep = TRUE;
        if (tempChar != 'C' && tempChar != 'G' && tempChar != 'T')  // Must be 'c', 'g', or 't'
        {
                cerr << "ERROR: Invalid option: " << argv[1] << endl;
         cerr << "GolfScore will quit now\n";
        return -1;
        }
   }

   if (strlen(argv[1]) > 3)			// If there is a third character...
   {
        tempChar = argv[1][3];	// Get the 3rd character in upper case
      if (tempChar == 'c')			// ...check third character of first parameter
                cRep = TRUE;
        if (tempChar == 'g')
                gRep = TRUE;
        if (tempChar == 't')
                tRep = TRUE;
        if (tempChar != 'c' && tempChar != 'g' && tempChar != 't')	// Must be 'c', 'g' or 't'
        {
                cerr << "ERROR: Invalid option: " << argv[1] << endl;
         cerr << "GolfScore will quit now\n";
        return -1;
        }
   }

   if (strlen(argv[1]) > 4)			// Check for more than 3 options
        cout << "WARNING: Options beyond the first 3 will be ignored\n";

   fin.open(argv[2]);					// Attempt to open the input file
   if (!fin)                        // If attempt failed, quit
   {
        cerr << argv[0] << " cannot access " << argv[2] << endl;
      return -2;
   }

// **************************************
// ***	 Parsing the Course records   ***
// **************************************
   fin.getline(buffer, BUF_SIZE); 		 // Read the very first line
   if (buffer[0] != ' ')                 // First character should be blank
   {
          cerr << "ERROR: First record must have a blank in column 1; GolfScore will terminate\n";
      return -2;
   }
   while (fin != 0 && buffer[0] == ' ')
   {
          duplicate = 0;                     // Flag used to indicate duplicate or too many Course records
//   	  strncpy(tempID, &buffer[19], 1);	 // Get the Course ID in ASCII
      tempID = buffer[19];
          if (coursecount > 0)				 // Now see if we already have a course record
      {                                  // for this Course ID
         for (int j=0; j<coursecount; j++)
                if (courses[j]->ID == tempID)   // Compare stored ID's to new ID
            {
                   duplicate = 1;           // A match! Ignore this record
               cerr << "WARNING: Duplicate Course ID -- the record will be ignored\n";
               cerr << "Here's the record: " << buffer << endl;
            }
      }
      if (coursecount == COURSEMAX && duplicate != 1)  // If we already have the maximum number
                                                                                                   // of courses, ignore this one
      {
         duplicate = 1;
         cerr << "WARNING: We already have " << COURSEMAX << " courses\n";
         cerr << "Course " << tempID << " will be ignored\n";
      }
      if (duplicate == 0)				 // A new course ID
      {
                 courses[coursecount] = new(Course); // Reserve some space for the next course
                 strncpy(courses[coursecount]->name, &buffer[1], 18); // Save the course's name
// NOTE: The following is an error for the class -- '17' should be '18',
//       so the last possible character of the Course Name gets truncated
         courses[coursecount]->name[17] = '\0';	   // String terminator
         courses[coursecount]->ID = tempID;		   // Save the course's identifier
         for (int k=0; k<18; k++)   			   // Save all 18 par values
         {
                strncpy(temp, &buffer[k+20], 1);
                temp[1] = '\0';
                if (isdigit(temp[0]) == 0)   // Par value must be a digit
            {
               cerr << "Par value " << k+1 << " for course " << tempID << " is not a digit\n";
               cerr << "GolfScore will terminate\n";
               fin.close();
               return -3;
            }
                                courses[coursecount]->par[k] = atoi(temp);
            if (courses[coursecount]->par[k] < 3 || courses[coursecount]->par[k] > 5)
            {
                cerr << "Par value " << k+1 << " for course " << tempID << " is not 3, 4 or 5\n";
               cerr << "GolfScore will terminate\n";
 //              delete(courses);
 //              delete(golfers);
               fin.close();
               return -3;
            }
                        }
        coursecount++;								// Increase the count of the number of courses
      }
      fin.getline(buffer, BUF_SIZE);			// Read the next record
   }

   if (fin == 0)										// If it's not 0, we have just read the delimiter record
   {
        cerr << "There are no golfer records; GolfScore will terminate\n";
      fin.close();
      return -5;
   }

//	**************************************
// ***	Parsing the Golfer records   ***
// **************************************
   fin.getline(buffer, BUF_SIZE); 		// Read what should be the first golfer record
   if (fin == 0)
   {
          cerr << "ERROR:There are no golfer records; GolfScore will terminate\n";
      fin.close();
      return -5;
   }

   if (buffer[0] != ' ')
   {
          cerr << "ERROR: First golfer record must have a blank in column 1; GolfScore will terminate\n";
      fin.close();
      return -2;
   }

   while (fin != 0 && buffer[0] == ' ')	// Keep reading until the file is empty or
   {										// we reach a delimiter record
          tempID = buffer[1];					// Get the Course ID
      strncpy(tempName, &buffer[9], 20);
      tempName[20] = '\0';    				// Get the golfer's name
      duplicate = 0;

          if (golfercount > 0)   					// Now see if we already have a golfer record
      {                                     // for this golfer and Course ID
         for (int j=0; j<golfercount; j++)
         {
            if (strcmp(tempName, golfers[j]->name) == 0)  // Golfer already has a structure
                {
                           for (int k=0; k<golfers[j]->cnt; k++)   	// Check for a matching Course ID
               {
                           if (golfers[j]->IDs[k] == tempID)
               {
                  duplicate = 1;
                  cerr << "Golfer has already played this course -- the record will be ignored\n";
                  cerr << "Here's the record: " << buffer << endl;
               }
            }
            if (duplicate == 0)   					// No matching Course ID, but golfer does have a structure
            {
               golfers[j]->IDs[golfers[j]->cnt] = tempID;	// Store the Course ID
                           for (int p=0; p<18; p++)			// Store the hole-by-hole scores
                           {
                              strncpy(temp, &buffer[p+30], 1);
                                  temp[1] = '\0';
                  if (isdigit(temp[0]) == 0)
                  {
                     cerr << "Golfer " << tempName << " has a non-digit score on Course "
                        << tempID << endl;
                     cerr << "GolfScore will terminate\n";
                     return -3;
                  }
                                  golfers[j]->scores[golfers[j]->cnt][p] = atoi(temp);
                           }
                           golfers[j]->scores[golfers[j]->cnt][18] = 0;		// Initialize the total score
                           for (int q=0; q<18; q++)
                              golfers[j]->scores[golfers[j]->cnt][18] += golfers[j]->scores[golfers[j]->cnt][q];
                           golfers[j]->scores[golfers[j]->cnt][19] = 0;	// Initialize ranking score
                   courseIndex = -1;
                   for (int r=0; r<coursecount; r++) 		// Get the course ID for this round
               {
                          if (courses[r]->ID == golfers[j]->IDs[golfers[j]->cnt])
                     courseIndex = r;
               }
                              if (courseIndex == -1)           		// No matching course ID was found
                          {
                         duplicate = 1;								// We cannot continue with this record
                                 cerr << "Error:  No course ID match for this golfer; record will be ignored\n";
                     cerr << "Here's the record: " << buffer << endl;
                  }
                  if (duplicate == 0)
                  {
                         for (int s=0; s<18; s++)   				// Compute the Ranking score
                                 {
                                tempScore = golfers[j]->scores[golfers[j]->cnt][s];
                        tempPar = courses[courseIndex]->par[s];
                        if (tempScore > tempPar)
                                   golfers[j]->scores[golfers[j]->cnt][19] += 0;
                                        if (tempScore == tempPar)
                                   golfers[j]->scores[golfers[j]->cnt][19] += 1;
                                        if (tempScore == (tempPar-1))
                                   golfers[j]->scores[golfers[j]->cnt][19] += 2;
                                        if (tempScore == (tempPar-2))
                                   golfers[j]->scores[golfers[j]->cnt][19] += 3;
                                        if (tempScore <= (tempPar-3))
                                   golfers[j]->scores[golfers[j]->cnt][19] += 6;
                                 }
                     golfers[j]->cnt++;		// Increment count of courses played
                         duplicate = 1;			// To bypass the rest of this loop
                  }
               }
            } // End "if (strcmp...)"
         }	// End "for (int j=0...)"
      } // End "if (golfercount > 0)"
      if (golfercount == GOLFERMAX && duplicate != 1)  // If we already have the maximum number
                                                                                                   // of golfers, ignore this one
      {
         duplicate = 1;
         cerr << "WARNING: We already have " << GOLFERMAX << " golfers\n";
         cerr << "Golfer " << tempName << " will be ignored\n";
      }
      if (duplicate == 0 && golfercount < GOLFERMAX)	// No duplicate was found of any kind for this golfer
          {
         golfers[golfercount] = new(Golfer);
                 strcpy(golfers[golfercount]->name, tempName);
         golfers[golfercount]->IDs[0] = tempID;			// Store the course ID for this round
         for (int k=0; k<18; k++)       							// Store the hole-by-hole scores for this round
         {
                strncpy(temp, &buffer[k+30], 1);
                temp[1] = '\0';
            if (isdigit(temp[0]) == 0)
            {
               cerr << "Golfer " << tempName << " has a non-digit score on Course "
                        << tempID << endl;
               cerr << "GolfScore will terminate\n";
               return -3;
            }
            golfers[golfercount]->scores[0][k] = atoi(temp);
         }
         golfers[golfercount]->scores[0][18] = 0;				// Initialize score for the round
         for (int m=0; m<18; m++)                           // Compute total score by repeated additions
                golfers[golfercount]->scores[0][18] += golfers[golfercount]->scores[0][m];
         golfers[golfercount]->scores[0][19] = 0;				// Initialize ranking score for this round
         // Now determine the ranking score:
         courseIndex = -1;
         for (int r=0; r<coursecount; r++)						// Find the matching golf course
                if (courses[r]->ID == golfers[golfercount]->IDs[0])
               courseIndex = r;
         if (courseIndex == -1)                             // No matching course ID was found
         {
                duplicate = 1;
                cerr << "Error:  No course ID match for this golfer; record will be ignored\n";
            cerr << "Here's the record: " << buffer << endl;
         }
         if (duplicate == 0)
         {
                for (int s=0; s<18; s++)         						// Compute the Ranking score
                {
                   if (golfers[golfercount]->scores[0][s] > courses[courseIndex]->par[s])
                  golfers[golfercount]->scores[0][19] += 0;
                   if (golfers[golfercount]->scores[0][s] == courses[courseIndex]->par[s])
                  golfers[golfercount]->scores[0][19] += 1;
                   if (golfers[golfercount]->scores[0][s] == (courses[courseIndex]->par[s]-1))
                  golfers[golfercount]->scores[0][19] += 2;
                   if (golfers[golfercount]->scores[0][s] == (courses[courseIndex]->par[s]-2))
                  golfers[golfercount]->scores[0][19] += 3;
                   if (golfers[golfercount]->scores[0][s] <= (courses[courseIndex]->par[s]-3))
                  golfers[golfercount]->scores[0][19] += 6;
                }
                golfers[golfercount]->cnt = 1;	// Initialize the count of courses played by this golfer
                golfercount++;
         }
      }
      fin.getline(buffer, BUF_SIZE);
   }
   fin.close();   							// Close the input file

// ******************************************
// ***	Determine the output directory    ***
// ******************************************
   if (argc >= 4)							// If a directory is supplied, use it
      strcpy(tempDir, argv[3]);
   else
   {                                    // None supplied -- use execution directory
      strcpy(tempDir, ".\\");
      if (strchr(argv[2], '\\') != NULL)
      {
      strncpy(tempDir, argv[2], strlen(argv[2]) - strlen(strrchr(argv[2], '\\'))+1);  // The +1 gets the '\' character
      }
   }
//   	    strncpy(tempDir, argv[0], strlen(argv[0]) - strlen(strchr(argv[0], '\\')));
   if (tempDir[strlen(tempDir) - 1] != '\x5c')					// Make sure it ends in a backslash
   {
      tempDir[strlen(tempDir)] = '\x5c';
          tempDir[strlen(tempDir)+1] = '\0';
   }
   for (int i=0; i<golfercount; i++)     						// Compute each golfer's Ranking score
   {
      golfers[i]->rankScore = 0;						// Initialize the Ranking Score total
      for (int k=0; k<golfers[i]->cnt; k++)
        golfers[i]->rankScore += golfers[i]->scores[k][19];
   }

// Now we will do a sort of the golfers based on Ranking score

	for (int s=0; s<golfercount; s++)
	{
		golferSort[s][0] = golfers[s]->rankScore;	// Save the Ranking score in the first "column"
		golferSort[s][1] = s;						// Save the index in the second "column"
	}
   for (int q=0; q<golfercount; q++)               // Sort the golfers by Ranking Score, making
          for (int r=q; r<golfercount; r++)			// sure to carry the index along, too

         if (golferSort[q][0] < golferSort[r][0])
         {
            temp1=golferSort[q][0];
            temp2=golferSort[q][1];
            golferSort[q][0]=golferSort[r][0];
            golferSort[q][1]=golferSort[r][1];
            golferSort[r][0]=temp1;
            golferSort[r][1]=temp2;
         }

// Determine each golfer's Rank
   int	Rank = 1;
   golfers[golferSort[0][1]]->Rank = 1; 				// We go through the sorted golfers, bumping the
        for (int a=1; a<golfercount; a++)					// rank when a new score is reached
   {
        if (golferSort[a][0] < golferSort[a-1][0])
                Rank = a+1;
      golfers[golferSort[a][1]]->Rank = Rank;
   }
// And here we do a sort of the golfers based on name

	for (int t=0; t<golfercount; t++)
	{
		strcpy(nameSort[t], golfers[t]->name);			// Save the name
		indexSort[t] = t;							  	// Save the index
	}

	for (int u=0; u<golfercount; u++)     				// Sort the golfers by name, carrying along
	   for (int v=u; v<golfercount; v++)				// the index, too
       {
          if (strcmp(nameSort[u], nameSort[v]) > 0)
          {
             strcpy(string1, nameSort[u]);
             temp2=indexSort[u];
             strcpy(nameSort[u], nameSort[v]);
             indexSort[u]=indexSort[v];
             strcpy(nameSort[v], string1);
             indexSort[v]=temp2;
           }
       }
// **********************************************
// ***   Generate Tournament Ranking Report   ***
// **********************************************

	if (tRep == TRUE)
	{
//  	strcpy(tOutfile, '\0');
          strcpy(tOutfile, tempDir);
      strcat(tOutfile, "trank.rep");
      tfile.open(tOutfile);											// Let's see if the file already exists
      if (tfile)
          {
        cout << "File " << tOutfile << " already exists.\n";
        cout << "Do you want to overwrite it? (Y/N)\n";
        cin >> answer;
        if (toupper(answer) == 'N')
                tRep = FALSE;
         tfile.close();
      }
   }
   if (tRep == TRUE)          						// We will be doing a Tournament Ranking report
   {
      fout.open(tOutfile);								// Of course, if we cannot write to the file,
          if (!fout)											// we're done
          {
                cerr << argv[0] << " cannot access " << argv[3] << endl;
 //        delete(courses);                       // Give memory back to the heap
 //        delete(golfers);
              return -2;
          }
      fout << "\t\t Tournament Ranking Report\n";

      for (int i=0; i<golfercount; i++)      	// Generate the lines of the report
      {
        fout << golfers[golferSort[i][1]]->name << "\t\tRank:  " << golfers[golferSort[i][1]]->Rank << endl;
         for (int j=0; j<coursecount; j++)
                for (int k=0; k<golfers[golferSort[i][1]]->cnt; k++)
                        if (golfers[golferSort[i][1]]->IDs[k] == courses[j]->ID)
                        fout << "\tCourse ID: " << courses[j]->ID << "\tRaw score: " << golfers[golferSort[i][1]]->scores[k][18]
                                << "\tRanking score:\t" << golfers[golferSort[i][1]]->scores[k][19] << endl;
        fout << "\t\t\t\t\t\tTOTAL:\t" << golfers[golferSort[i][1]]->rankScore << endl;

      }
      fout.close();
   }

//	**************************************
// ***	Generate the Golfer Report   ***
// **************************************

	if (gRep == TRUE)
	{
	  strcpy(gOutfile, tempDir);
      strcat(gOutfile, "golfer.rep");
      gfile.open(gOutfile);											// Let's see if the file already exists
      if (gfile)
          {
         cout << "File " << gOutfile << " already exists.\n";
         cout << "Do you want to overwrite it? (Y/N)\n";
         cin >> answer;
         if (toupper(answer) == 'N')
                gRep = FALSE;
         gfile.close();
      }
   }
   if (gRep == TRUE)  							// We will be generating a Golfer report
   {
      fout.open(gOutfile);						// If we can't even open the file, quit
          if (!fout)
          {
                 cerr << argv[0] << " cannot access " << argv[3] << endl;
             return -2;
          }
      fout << "\t\t\t Golfer Report\n"; 	// Generate the lines of the report
      for (int i=0; i<golfercount; i++)
      {
        fout << nameSort[i] << "\t\tRank:  " << golfers[indexSort[i]]->Rank << endl;
         for (int j=0; j<coursecount; j++)
                for (int k=0; k<golfers[indexSort[i]]->cnt; k++)
                        if (golfers[indexSort[i]]->IDs[k] == courses[j]->ID)
                        fout << "\tCourse ID: " << courses[j]->ID << "\tRaw score: " << golfers[indexSort[i]]->scores[k][18]
                                << "\tRanking score: " << golfers[indexSort[i]]->scores[k][19] << endl;
        fout << "\t\t\t\t\t\tTOTAL: " << golfers[indexSort[i]]->rankScore << endl;

      }
      fout.close();
   }

// **************************************
// ***   Generate the Course Report   ***
// **************************************

   if (cRep == TRUE)
   {
          strcpy(cOutfile, tempDir);
      strcat(cOutfile, "course.rep");
      cfile.open(cOutfile);				    // Let's see if the file already exists
      if (cfile)
          {
         cout << "File " << cOutfile << " already exists.\n";
         cout << "Do you want to overwrite it? (Y/N)\n";
         cin >> answer;
         if (toupper(answer) == 'N')
                cRep = FALSE;
         cfile.close();
      }
   }
   if (cRep == TRUE) 						// We will be generating a Course report
   {
      fout.open(cOutfile);
          if (!fout)                            // If we cannot open the output file, quit
          {
             cerr << argv[0] << " cannot access " << cOutfile << endl;
             return -2;
          }
      fout << "\t\t\t Course Report\n";
      for (int ix=0; ix<coursecount; ix++)	// Go through all the golf courses
      {
         fout << "\nCourse:\t" << courses[ix]->name << "\tID: \t" << courses[ix]->ID << endl;

             for (int b=0; b<golfercount; b++)	// For each course, cycle through all the golfers
         {
            int indx=-1;
            for (int c=0; c<golfers[b]->cnt; c++) // Go through each course this golfer played
            {
               if (golfers[b]->IDs[c] == courses[ix]->ID)
                  indx = c;
            }
            if (indx != -1)
//         		fout << "\t" << nameSort[b] << "\tHole score: " <<
//						golfers[indexSort[b]]->scores[indx][19] << "\tRank score: " <<
//						golfers[indexSort[b]]->scores[indx][20] << endl;
                    {
               fout << "\t" << golfers[b]->name << "\t" <<
                              golfers[b]->scores[indx][0] <<
                  golfers[b]->scores[indx][1] <<
                              golfers[b]->scores[indx][2] <<
                              golfers[b]->scores[indx][3] <<
                              golfers[b]->scores[indx][4] <<
                              golfers[b]->scores[indx][5] <<
                  golfers[b]->scores[indx][6] <<
                  golfers[b]->scores[indx][7] <<
                  golfers[b]->scores[indx][8] <<
                  golfers[b]->scores[indx][9] <<
                              golfers[b]->scores[indx][10] <<
                              golfers[b]->scores[indx][11] <<
                              golfers[b]->scores[indx][12] <<
                              golfers[b]->scores[indx][13] <<
                              golfers[b]->scores[indx][14] <<
                              golfers[b]->scores[indx][15] <<
                  golfers[b]->scores[indx][16] <<
                  golfers[b]->scores[indx][17] << endl;
                           fout << "\t\t\t\tHole score: " << golfers[b]->scores[indx][18] << "\tRank score: " <<
                              golfers[b]->scores[indx][19] << endl;
            }
         }
      }
      fout.close();
   }

   cout << "\nGolfScore is finished processing\n";
   return 0;
}
