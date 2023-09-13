/*
Name: Lim Tze Yang
Assignment Title: Grocery Store Database
Date: 4/12/2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLINELEN 256

typedef struct item {
	int id;
	char name[20];
	double cost;
	char category;
	struct item *next;
} item;

typedef item *itemP;

void getValidInput(int, void*, char*); //function to get valid input 
int checkFileExtension(char*); //function to check for file extension from input and return its validity
void openFile(char*, FILE**, int); //function to open file and display errors in opening file
int loadFile(FILE*, char*, itemP*); //function to load data from file into database and return items loaded
void addItem(itemP*, int, char*, double, char); //function to add items to database
int idExists(itemP, int); //function to check for existing id in database and return 1 if id exists
int nameExists(itemP, char*); //function to check for existing name in database and return 1 if name exists
void deleteItem(itemP*, int); //function to delete item from database and return 1 if delete was successful
itemP searchItem(itemP, char*, int); //function to search for an item in database and return pointer to item
void swapItems(itemP, itemP); //function to swap position of items in database
void sortDatabase(itemP, int, int, int); //function to sort database items
void displayDatabase(itemP); //function to display database items
int saveFile(FILE*, itemP); //function to save data from database to file and return items saved
void upperStr(char*); //function to convert string to uppercase

int main() {
    itemP headPtr = NULL, itemPtr;
	int itemCount, choice = 0, iID, saveFlag, sOrder, sBy;
	char lfName[MAXLINELEN], sfName[MAXLINELEN], iName[20], iCategory, saveResponse[4];
	double iCost;
	FILE *fPtr = NULL;

    puts("Welcome to the grocery store!");	
	
	//loop to get and open a valid file from user input
	do {
		getValidInput(0, lfName, "Please input the file you'd like to load into stock:");

		if(checkFileExtension(lfName))
			openFile(lfName, &fPtr, 1);		
	} while(fPtr == NULL);

	itemCount = loadFile(fPtr, lfName, &headPtr);
    printf("\n%d items have been properly loaded into database from %s!\n", itemCount, lfName);
	saveFlag = 0; //initialise save flag
	fclose(fPtr); //close file
	fPtr = NULL; //reset file pointer to NULL to use for save file later	

	//loop forever
	for(;;) {
		switch(choice) {			
			case 0:				
				getValidInput(1, &choice, "\nPlease make one of the following selections:\n1. Add new item\n2. Delete item\n3. Change the cost of an item\n4. Search for item\n5. Display inventory details\n6. Quit");
				break;
			case 1:
				getValidInput(2, &iID, "\nWhat is the ID number of the item to add?");

				//check for existing id in database
				if(idExists(headPtr, iID)) {
					printf("Sorry an item with the ID number %d is already in the database.\n\n", iID);
					choice = 0; //go back to main menu
					break;
				}

				getValidInput(3, iName, "\nWhat is the name of the item to be added?");

				//check for existing name in database
				if(nameExists(headPtr, iName)) {
					printf("Sorry an item with the name %s is already in the database.\n\n", iName);
					choice = 0; //go back to main menu
					break;
				}

				getValidInput(4, &iCost, "\nWhat is the cost of the item to be added?");

				do {
					getValidInput(5, &iCategory, "\nWhat is the product category of the item to be added?");

					if(toupper(iCategory) != 'M' && toupper(iCategory) != 'P' && toupper(iCategory) != 'D' && toupper(iCategory) != 'C' && toupper(iCategory) != 'N')
						printf("[ERROR] %c is not a valid category!\n\n", iCategory);
					
				} while(toupper(iCategory) != 'M' && toupper(iCategory) != 'P' && toupper(iCategory) != 'D' && toupper(iCategory) != 'C' && toupper(iCategory) != 'N');

				addItem(&headPtr, iID, iName, iCost, toupper(iCategory));
				printf("%s has been added to the database.\n", iName);
				itemCount++; //update item count
				printf("There are now %d items in the database.\n\n", itemCount);
				saveFlag = 1; //update save flag
				choice = 0; //go back to main menu
				break;
			case 2:		
				getValidInput(2, &iID, "\nWhat is the ID number of the product to delete?");

				if(idExists(headPtr, iID)) {
					deleteItem(&headPtr, iID);
					saveFlag = 1; //update save flag
					itemCount--; //update item count					
					printf("%d items remain in the database.\n\n", itemCount);
				} else
					puts("Sorry, there is no item in the database with that ID number.\n");

				choice = 0; //go back to main menu
				break;
			case 3:	
				getValidInput(2, &iID, "\nWhat is the ID number of the item in question?");
				
				//check if item with that id number exists in database
				if(idExists(headPtr, iID)) {
					char prompt[MAXLINELEN];

					itemPtr = searchItem(headPtr, "", iID); //get itemPtr by searching with ID
					sprintf(prompt, "What is the new price for %s?", itemPtr->name);
					getValidInput(4, &iCost, prompt); 
					itemPtr->cost = iCost; //update item cost
					printf("%s now costs $%.2lf.\n\n", itemPtr->name, itemPtr->cost); //indicate successful save
					saveFlag = 1;
				}
				else
					printf("Sorry, there is no item in the database with ID number %d.\n\n", iID); //indicate item does not exist in database
				
				choice = 0; //go back to main menu
				break;
			case 4:
				getValidInput(3, iName, "\nWhich item would you like to search for?");

				if(nameExists(headPtr, iName)) {
					itemPtr = searchItem(headPtr, iName, -1); //get pointer to item from function
					puts("Here is the product information:");
					printf("%d %s %.2lf %c\n\n", itemPtr->id, itemPtr->name, itemPtr->cost, itemPtr->category);
				} else
					printf("Sorry, we do not have %s in the store.\n\n", iName);

				choice = 0; //go back to main menu
				break;
			case 5:
				do {
					getValidInput(1, &sOrder, "\nPlease make one of the following selections:\n1.Display unsorted\n2.Display in ascending order\n3.Display in descending order");

					if(sOrder < 1 || sOrder > 3)
						printf("[ERROR] %d is not a valid selection!\n\n", sOrder);

				} while(sOrder < 1 || sOrder > 3);

				if(sOrder == 1) {
					displayDatabase(headPtr);
					choice = 0; //go back to main menu
				} else {
					do {
						getValidInput(1, &sBy, "\nPlease make one of the following selections:\n1.Sort by ID\n2.Sort by name\n3.Sort by cost\n4.Sort by category");

						if(sBy < 1 || sBy > 4)
							printf("[ERROR] %d is not a valid selection!\n\n", sOrder);

					} while(sBy < 1 || sBy > 4);
					
					//sort database if not empty
					if(headPtr != NULL) {
						sortDatabase(headPtr, itemCount, sOrder, sBy);
						saveFlag = 1; //update save flag
					}
					
					displayDatabase(headPtr);
					choice = 0; //go back to main menu
				}

				break;
			case 6:
				if(saveFlag) {
					char tempResponse[4];

					getValidInput(6, saveResponse, "Would you like to save the changes made to the database?");
					strcpy(tempResponse, saveResponse);
					upperStr(tempResponse); //convert response to uppercase for case insensitive comparisons

					if(!strcmp(tempResponse, "YES") || !strcmp(tempResponse, "Y") || atoi(tempResponse)) {
						do {
							getValidInput(0, sfName, "What file would you like to save the new database?");

							if(checkFileExtension(sfName)) {
								if(strcmp(sfName, lfName))
									openFile(sfName, &fPtr, 2);
								else
									puts("[ERROR] You cannot save to the original data file.\n");
							}							
						} while(fPtr == NULL);

						printf("%d items have been properly saved into %s from database. Thanks for shopping!\n", saveFile(fPtr, headPtr), sfName);
					} else {
						if(strcmp(tempResponse, "NO") && strcmp(tempResponse, "N") && atoi(tempResponse)) {
							printf("[ERROR] %s is not a valid response!\n\n", saveResponse);
							choice = 6;
						}
					}
				}

				puts("Quitting program...");
				return 0;
				break;
			default:
				printf("[ERROR] %d is not a valid selection!\n\n", choice);
				choice = 0; //go back to main menu
				break;
		}
	}
}

int checkFileExtension(char *filename) {
	int valid = 0;
	char *extPtr;

   	if((extPtr = strrchr(filename, '.')) == NULL) {
		strcat(filename, ".txt"); //no file extension inputted, concatenate file extension
		valid = 1;
	} else {
    	//file is not a .txt file, display error and advice
		if(strcmp(extPtr, ".txt")) {			
			printf("[ERROR] %s is not a text file.\n", filename);
			puts("Please ensure that the file you have inputted is a text file.\n");
   		} else
		   	valid = 1;
	}
		   

	return valid;
}

void getValidInput(int type, void *iPtr, char *prompt) {
	int valid;
	char buffer[MAXLINELEN];

	switch(type) {
		//get valid file name input
		case 0:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, MAXLINELEN, stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%s", (char *)iPtr);

				if(!valid || !strlen(buffer))
					printf("[ERROR] %s is not a valid file!\n\n", buffer);
			} while(!valid || !strlen(buffer));

			break;
		//get valid menu selection input
		case 1:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%d", (int *)iPtr); //determine validity of input and assign input to designated pointer if valid

				if(!valid)
					printf("[ERROR] %s is not a valid selection!\n\n", buffer);
			} while(!valid);

			break;
		//get valid item ID input
		case 2:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%d", (int *)iPtr); //determine validity of input and assign input to designated pointer if valid

				//display error if invalid or empty input
				if(!valid || atoi(buffer) < 0 || !strlen(buffer))
					printf("[ERROR] %s is not a valid item ID!\n\n", buffer);
			} while(!valid || atoi(buffer) < 0 || !strlen(buffer));
			
			break;
		//get valid item name input
		case 3:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%19s", (char *)iPtr); //determine validity of input and assign input to designated pointer if valid

				//display error if invalid or empty input
				if(!valid || !strlen(buffer))
					printf("[ERROR] %s is not a valid item name!\n\n", buffer);
			} while(!valid || !strlen(buffer));
			
			break;
		//get valid cost input
		case 4:
			do {
				puts(prompt);
				printf("$");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%lf", (double *)iPtr); //determine validity of input and assign input to designated pointer if valid

				//display error if invalid or empty input
				if(!valid || atoi(buffer) < 0 || !strlen(buffer))
					printf("[ERROR] %s is not a valid cost!\n\n", buffer);
			} while(!valid || atoi(buffer) < 0 || !strlen(buffer));
			
			break;
		//get valid category input
		case 5:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%c", (char *)iPtr); //determine validity of input and assign input to designated pointer if valid

				//display error if invalid or empty input
				if(!valid || !strlen(buffer))
					printf("[ERROR] %s is not a valid category!\n\n", buffer);
			} while(!valid || !strlen(buffer));
			
			break;

		//get valid response
		case 6:
			do {
				puts(prompt);
				printf("> ");
				memset(buffer, 0, strlen(buffer)); //reset buffer to validate new input
				fgets(buffer, sizeof(buffer), stdin); //assign input to buffer for validation
				buffer[strlen(buffer)-1] = '\0'; //replace remove last newline character from string returned by fgets()
				valid = sscanf(buffer, "%s", (char *)iPtr); //determine validity of input and assign input to designated pointer if valid

				//display error if invalid or empty input
				if(!valid || !strlen(buffer))
					printf("[ERROR] %s is not a valid response!\n\n", buffer);
			} while(!valid || !strlen(buffer));
			
			break;
	}	
}

void openFile(char *filename, FILE **fPtr, int mode) {
	switch(mode) {
		//open file to load into database
		case 1:	
        	if((*fPtr = fopen(filename, "r")) == NULL) {
				//could not open file, display error and advice
       	  		printf("[ERROR] %s could not be opened and read from.\n", filename);
				puts("Please ensure that the file you have inputted exists and can be read from.\n");
			}						

			break;
		//open file to save database to
		case 2:			
			if((*fPtr = fopen(filename, "w")) == NULL)
				//could not open file, display error and advice
       			printf("[ERROR] Could not save to %s\n", filename);

			break;
	}
}

int loadFile(FILE *fPtr, char *filename, itemP *head) {
    int tid, count = 0;
	char tname[20];
	double tcost;
	char tcategory;

	//iterate through lines in file
    while(fscanf(fPtr, "%d %19s %.2lf %c", &tid, tname, &tcost, &tcategory) == 4) {
		if(!idExists(*head, tid) && !nameExists(*head, tname)) {
			if(tid > 0 && tcost > 0) {
				if(toupper(tcategory) == 'M' || toupper(tcategory) == 'P' || toupper(tcategory) == 'D' || toupper(tcategory) == 'C' || toupper(tcategory) == 'N') {
        			addItem(head, tid, tname, tcost, tcategory); //load item into database
					count++; //update loaded item count
				} else {
					printf("[ERROR] Item %s with ID number %d was not loaded from %s as it has an invalid category.\n", tname, tid, filename);
				}
			} else {
				printf("[ERROR] Item %s with ID number %d was not loaded from %s as it has an invalid ID number or cost.\n", tname, tid, filename);
			}
		} else
			printf("[ERROR] Item %s with ID number %d was not loaded from %s as it is a duplicate.\n", tname, tid, filename);
    }

	return count;
}

void addItem(itemP *head, int id, char *name, double cost, char category) {
    itemP newPtr, currPtr;

    newPtr = (itemP) malloc(sizeof(item)); //create new entry

    if(newPtr != NULL) {
        //assign values to new entry
        newPtr->id = id;
        strcpy(newPtr->name, name);
        newPtr->cost = cost;
        newPtr->category = category;
        newPtr->next = NULL;		

        //insert entry
        if(*head == NULL) {
            *head = newPtr; //insert at beginning if database is empty
        } else {
            currPtr = *head;

            while(currPtr->next != NULL) {
                currPtr = currPtr->next; //move pointer to end of database
            }

            currPtr->next = newPtr; //insert at end if database was already populated
        }
    } else
        printf("[ERROR] %s could not be added\n", name); //display error if fail to add item to database
}

int idExists(itemP head, int id) {
	int dupe = 0;
	itemP currPtr = head;

	//iterate through items in database
	while(currPtr != NULL) {
		if(currPtr->id == id) {
			dupe = 1; //update dupe flag
			break;
		}

		currPtr = currPtr->next; //move to next item
	}

	return dupe;
}

int nameExists(itemP head, char *name) {
	int dupe = 0;
	char temp1[20], temp2[20];
	itemP currPtr = head;
	strcpy(temp1, name);
	upperStr(temp1);

	//iterate through items in database 
	while(currPtr != NULL) {
		strcpy(temp2, currPtr->name);
		upperStr(temp2);

		if(strcmp(temp1, temp2) == 0) {
			dupe = 1; //update dupe flag
			break;
		}

		currPtr = currPtr->next; //move to next item
	}

	return dupe;
}

void deleteItem(itemP *head, int id) {
	itemP currPtr = *head, prevPtr = NULL, tempPtr, lastPtr = *head, lastPrevPtr = NULL;

	//get pointer to last and second last item in database
	while(lastPtr->next != NULL) {
		lastPrevPtr = lastPtr;
		lastPtr = lastPtr->next;
	}

	//check for item in database with matching id
	while(currPtr != NULL) {

		//delete and replace item that matches id with last item in database
		if(currPtr->id == id) {
			tempPtr = currPtr; //assign current item to temp variable

			if(prevPtr != NULL)
				prevPtr->next = lastPtr; //assign last item to be next item of previous item if current item is not first item
			else
				*head = lastPtr; //make last item first item if current item is first item

			if(currPtr != lastPrevPtr) {
				lastPrevPtr->next = NULL; //update second last item to be last item
				lastPtr->next = currPtr->next; //place last item in list in current item position
			} else
				lastPtr->next = NULL; //replace current item with last item if current item is second last item	

			printf("%s has been deleted from the database.\n", tempPtr->name);
			free(tempPtr); //free memory allocated to deleted item
			break;
		}

		prevPtr = currPtr;
		currPtr = currPtr->next;
	}
}

itemP searchItem(itemP head, char *name, int id) {
	itemP currPtr = head, result = NULL;
	char temp1[20], temp2[20];

	strcpy(temp1, name);
	upperStr(temp1);

	//iterate through items in database
	while(currPtr != NULL) {
		//search for name if id = -1
		if(id == -1) {
			strcpy(temp2, currPtr->name);
			upperStr(temp2);

			if(strcmp(temp1, temp2) == 0) {
				result = currPtr; //assign item pointer with matching name to result
				break;
			}
		} else {
			if(currPtr->id == id) {
				result = currPtr; //assign item pointer with matching id to result
				break;
			}
		}

		currPtr = currPtr->next; //move to next item
	}

	return result;
}

void swapItems(itemP item1, itemP item2) {
	int tid;
	char tname[20];
	double tcost;
	char tcategory;

	//assign values to temp variables
	tid = item1->id;
	strcpy(tname, item1->name);
	tcost = item1->cost;
	tcategory = item1->category;

	//assign item 2's values to item 1
	item1->id = item2->id;
	strcpy(item1->name, item2->name);
	item1->cost = item2->cost;
	item1->category = item2->category;

	//assign temp values to item 2
	item2->id = tid;
	strcpy(item2->name, tname);
	item2->cost = tcost;
	item2->category = tcategory;
}

void sortDatabase(itemP head, int items, int sortorder, int sortby) {
	int swap;
	itemP currPtr, prevPtr = NULL;

	switch(sortorder) {
		//sort ascending
		case 2:
			switch(sortby) {
				//sort by id
				case 1:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current id is greater than next id
							if(currPtr->id > currPtr->next->id) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by name
				case 2:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when difference between current name and next name is positive
							if(strcmp(currPtr->name, currPtr->next->name) > 0) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by cost
				case 3:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current cost is greater than next cost
							if(currPtr->cost > currPtr->next->cost) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by category
				case 4:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current category is greater than next category
							if(currPtr->category > currPtr->next->category) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
			}
			break;
		//sort descending
		case 3:
			switch(sortby) {
				//sort by id
				case 1:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current id is less than next id
							if(currPtr->id < currPtr->next->id) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by name
				case 2:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when difference between current name and next name is negative
							if(strcmp(currPtr->name, currPtr->next->name) < 0) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by cost
				case 3:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current cost is less than next cost
							if(currPtr->cost < currPtr->next->cost) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
				//sort by category
				case 4:
					//iterate through items database to compare and swap if necessary until no more swaps are made
					do {
						swap = 0;
						currPtr = head;

						while(currPtr->next != prevPtr) {
							//swap when current category is less than next category
							if(currPtr->category < currPtr->next->category) { 
								swapItems(currPtr, currPtr->next);
								swap = 1; //update swap flag
							}

							currPtr = currPtr->next; //move to next item
						}

						prevPtr = currPtr;
					} while(swap);

					break;
			}
			break;
	}
}

void displayDatabase(itemP head) {
	itemP currPtr = head;

	puts("_____________________________________________");
	puts("|ID   |Product            |Price   |Category|");
	puts("_____________________________________________");

	//iterate through items in database and display item information in tabular manner
	while(currPtr != NULL) {
		printf("|%-5d|%-19s|%-7.2lf |   %c    |\n", currPtr->id, currPtr->name, currPtr->cost, currPtr->category);
		currPtr = currPtr->next;
	}

	puts("_____________________________________________");
	printf("\n");
}

int saveFile(FILE *fPtr, itemP head) {
	int count = 0;
	itemP currPtr = head;

	//iterate through items in database and write item information to file
	while(currPtr != NULL) {
		fprintf(fPtr, "%d %s %.2lf %c\n", currPtr->id, currPtr->name, currPtr->cost, currPtr->category);
		count++;
		currPtr = currPtr->next;
	}

	return count;
}

void upperStr(char *str) {
	char *ptr;

	//iterate through characters in string 
	for(ptr=str; *ptr; ptr++) 
		if(*ptr >= 'a' && *ptr <= 'z') 
			*ptr -= 32; //convert lowercase to uppercase
}