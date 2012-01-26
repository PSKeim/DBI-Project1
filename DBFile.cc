#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <iostream>
#include <fstream>

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () {	
}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
	//Creates a file at f_path. 
	//Currently, that's all this does. Will figure out what needs to get put in that file, probably in Close
	//Also can create a meta-data file that indicates what f_type was. 
	
	if(f_type == heap){ //Handle for fType 0, or the Heap type. All other types (currently) result in a "Fail to create File" situation.
		f.Open(0,f_path); //Open file to the path given, let's assume file handles the "unable to be created" error?
		//Now to make the Metafile
		string metafile;
		metafile.append(f_path);
		metafile.append(".header");
		//Open and write out the type
		ofstream dbFile;
		dbFile.open(metafile.c_str(),ios::out);
		dbFile << f_type; //In this case, writes 0. If I need it to do more metadata later, I'll mess with it.
		dbFile.close();
		return 1;
	}

	return 0;
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
//Loads a bunch of data in from the file at loadpath.
//Stores it into the file.
//Adjusts pages as necessary.
       FILE *tableFile = fopen (loadpath, "r");
	//Code is more or less ripped from main.cc
        Record temp; //Holding variable
	
	// read in all of the records from the text file and see if they match
	// the CNF expression that was typed in
	int counter = 0; //Counter for debug. Take out of final product!
	int pageCounter = 0; //File needs a page offset to know where it is putting the page. This is it.
        while (temp.SuckNextRecord (&f_schema, tableFile) == 1) {
		counter++;//Debug part of loop, just making sure it works
		if (counter % 10000 == 0) {
			cout << counter << "\n";
		}
		//Right now Temp is the next record from our table file...
		if(p.Append(&temp) == 0){ //If the append function returns a 0, the append failed (page is full)
			//So we need to add the page to the file, and start again
			f.AddPage(&p, pageCounter);
			pageCounter++;
			p.EmptyItOut();
			if(p.Append(&temp) == 0){
				cerr << "Page repeatedly failed to append! Skipping record!";
			}
		}
       }
	//We need to add the final page to the File. 
	f.AddPage(&p,pageCounter);
	// Done? Maybe? I dunno.*/
}

int DBFile::Open (char *f_path) {
//Loads a previously saved DBFile in, somehow
	f.Open(1,f_path);
	return 1;
}

void DBFile::MoveFirst () {
//Move it to point to the first page in File. Or something?
//Use File's GetPage to get the first page (offset 1?), and set p as it.
//So... f.GetPage(&p,1);
p.EmptyItOut();
globalPageIndex = 0;
f.GetPage(&p,globalPageIndex);
}

int DBFile::Close () {
		//Does some closing stuff, such as closing the file, and writing out an extra metadata that I might need.
		f.Close();
		//dbFile.close();  // This'll be useful later if I leave it hanging open in Create
		return 1;
}

void DBFile::Add (Record &rec) {
//Okay, to Add, we must make sure that p = last page of the file.
//And then do shit to it. Namely, SCIENCE.
//Science is good.
//Wait. That doesn't make sense. We have to get the last page... and then add it? Because f.AddPage writes it out to file.
//But if we get the last page, does it zero it out to be nothing? MUST CHECK!

}

//This version of GetNext gets the next record from the File
int DBFile::GetNext (Record &fetchme) {
	//The first thing to do is fetch through the current page.
	//If the page returns 0, then we need to load the next page and get from there
	//If the GPI > f's size, then we've reached the end of the records

	if(p.GetFirst(&fetchme) == 0){ //Check to see if anything is returned by our current page p
			globalPageIndex++; //Update page to the next one
		if(globalPageIndex < f.GetLength()-1){ //If nothing is returned, we check to see if p is the last page
			f.GetPage(&p,globalPageIndex);
			p.GetFirst(&fetchme);
			return 1;
		}

		return 0;//No records left
	}

	return 1; 
}

//This version of GetNExt returns the next record that matches the CNF given
int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	
	if(p.GetFirst(&fetchme) == ){
	}

}

/*int DBFile::SafeIndex(){

}*/
