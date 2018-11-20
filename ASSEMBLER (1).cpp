#include<iostream>
#include<fstream>
#include<string>
#include<cstring>
#include<iomanip>
#include<sstream>
using namespace std;

string decToHexa(int n) 
{    
    if(n==0)
    	return "0";
    else{
	    char hexaDeciNum[100]; 	// char array to store hexadecimal number 
	    int i = 0; 
	    while(n!=0) 
	    {    
	        int temp  = 0; 	// temporary variable to store remainder 
	        temp = n % 16; 	// storing remainder in temp variable.
	        if(temp < 10) 
	        { 
	            hexaDeciNum[i] = temp + 48; 
	            i++; 
	        } 
	        else
	        { 
    	        hexaDeciNum[i] = temp + 55; 
        	    i++; 
        	} 
          
        	n = n/16; 
   		} 
    // printing hexadecimal number array in reverse order 
    	string hexNum = "";
    	for(int j=i-1; j>=0; j--)
        	hexNum+= hexaDeciNum[j];
    
    	return hexNum;
	}
}

string checkOpr(string str){
	string sub = str.substr(0,1);
	char ch = str.at(0);
	if(str == "AH" ||str == "AL" ||str == "BH" ||str == "BL" ||str == "CH" ||str == "AL" ||str == "DH" ||str == "DL" ||
	   str == "AX" ||str == "BX" ||str == "CX" ||str == "DX" ||str == "SI" ||str == "DI" ||str == "SP" ||
	   str == "CS" ||str == "DS" ||str == "SS" ||str == "ES")
	   return "REG";
	else if(sub=="[" || (ch>=65&&ch<=90) || (ch>=97&&ch<=122))
		return "MEM";
	else if(sub=="0" ||sub=="1" ||sub=="2" ||sub=="3" ||sub=="4" ||
	        sub=="5" ||sub=="6" ||sub=="7" ||sub=="8" ||sub=="9" )
	    return "IMM";
	else 
		return "na";
} 


string* split(string value){
	string *arr = new string[2];
	int pos = value.find(',');
	int lwlen = value.size()-pos;
	arr[0] = value.substr(0,pos);
	arr[1] = value.substr(pos+1,lwlen);
	return arr;
}

char** file;						//Entire file character by character
string** words;						//Words in enitre file
int rows = 0;

class symbolTable{
	string name;
	string offset;
	int size;
	string type;
	string segment;
	
	public:
		void setName(string name){
			this->name = name;
		}
		void setOffset(string offset){
			this->offset = offset;
		}
		void setSize(int size){
			this->size = size;
		}
		void setType(string type){
			this->type = type;
		}
		void setSegment(string segment){
			this->segment = segment;
		}
		
		string getName(){
			return name;
		}
		string getOffset(){
			return offset;
		}
		int getSize(){
			return size;
		}
		string getType(){
			return type;
		}
		string getSegment(){
			return segment;
		}
		
		string check(string word){
			int len = word.size();
			string wordSub = word.substr(len-1,1);
			if((word=="DB" || word=="DW" || word=="DD") || (word=="SEGMENT") || (word=="EQU") || (word=="LABEL") ||
			   (word=="db" || word=="dw" || word=="dd") || (word=="segment") || (word=="equ") || (word=="label")){
			   	return "DIRECTIVE";
			   }
			else if(word=="assume" || word=="ASSUME")
				return "ASSUME";
				
			else if(wordSub==":")
				return "LABEL";
				
			else if(word.substr(0,3)=="DUP" || word.substr(0,3)=="dup")
				return "DUP";
			
			else if(word == "INC" || word == "DEC" || word == "PUSH" ||
			        word == "POP" || word == "CLC")
			    return "OPCODE1";
				
			else if(word == "MOV" || word == "ADD" || word == "SUB" ||
				    word == "CMP" || word == "LEA" || word == "XOR")
				return "OPCODE2";
				
			else
				return "IDENTIFIER";
		}
		
		int checkSize(string word){
			if(word=="DB" || word=="db")
				return 1;
			else if(word=="DW" || word=="dw")
				return 2;
			else if(word=="DD" || word=="dd")
				return 4;
			else if(word=="SEGMENT" || word=="segment")
				return -5;
			else 
				return 999;
		}
		
		string checkSegment(string word,int i,int cw){
			static string tempSeg;
			if(word=="SEGMENT" || word=="segment"){
				tempSeg = words[i][cw-1];
				return "(ITSELF)";
			}
			else
				return tempSeg;
		}
		
		string checkType(string word){
			int s = checkSize(word);
			if(s==1 || s==2 || s==4)
				return "VAR";
			else if(s==-5)
				return "SEGMENT";
			else 
				return "na";
		}
		
		string checkOffset(string word , int sz=0 , bool dupFlag = false , bool arrayFlag = false , string addMode = "", int dispSize = 0){

			static int off = 0;
			int prevOffset = off;
			
			if(word=="SEGMENT" || word=="segment"){
				off = 0;
				return "0";
			}
			else if(word=="DB" || word=="db"){
				if(dupFlag)
					off += sz;
				else if(arrayFlag)
					off += sz+1;
				else
					off += 1;
				return decToHexa(prevOffset);
			}
			
			else if(word=="DW" || word=="dw"){
				if(dupFlag)
					off += sz;
				else if(arrayFlag)
					off += sz+2;
				else
					off += 2;
				return decToHexa(prevOffset);
			}
			
			else if(word=="DD" || word=="dd"){
				if(dupFlag)
					off += sz;
				else if(arrayFlag)
					off += sz+4;
				else
					off += 4;
				return decToHexa(prevOffset);
			}
			
			else if(word == "OPCODE1"){
				off+=1;
				return decToHexa(prevOffset);
			}
			
			else if(word == "OPCODE2"){
				if(addMode == "RTOR" || addMode == "RTOM")
					off+=2;
						
				if(addMode == "RTOM_DISP")
					off+=(2+dispSize);
					
				if(addMode == "IMMTOR")
					off+=4;
					
				if(addMode == "IMMTOR_DISP")
					off+=6;
					
				return decToHexa(prevOffset);
			}
			
			else if(word == "LABEL")
				return decToHexa(prevOffset);
			
		}
};


symbolTable records[20];
int symTabCounter=0;
int *wordsInLine;

void displayST(int symTabCounter){
	
	for(int i=0;i<100;i++) cout<<"=";
	cout<<endl<<setw(15)<<"NAME"<<setw(10)<<"SIZE"<<setw(15)<<"SEGMENT"<<setw(15)<<"TYPE"<<setw(15)<<"OFFSET"<<endl;
	for(int i=0;i<100;i++) cout<<"=";
		
	cout<<endl;
	for(int i=0;i<symTabCounter;i++){
		cout<<setw(15)<<records[i].getName()
			<<setw(10)<<records[i].getSize()
			<<setw(15)<<records[i].getSegment()
			<<setw(15)<<records[i].getType()
			<<setw(15)<<records[i].getOffset()<<endl;
	}
}

void buildSymbolTable(){
	int i=0,j;
	while(i<rows){
		j=0;
		while(j<wordsInLine[i]){
			string wordID=records[symTabCounter].check(words[i][j]);
			
			if(wordID=="DIRECTIVE"){			
						bool flag = false;						//for checking dup
						bool arrayFlag = false;
						int sz = 0;
						if(wordsInLine[i]>3)
							if(words[i][3].size()>=3)
							if(words[i][3].substr(0,3)=="DUP"){
								sz = records[symTabCounter].checkSize(words[i][j]);
								string dupSize = words[i][j+1];
								int ds=0;
								char ch;
								for(int x=0;x<dupSize.size();x++){
									ds*=10;
									ch = dupSize.at(x);
									ds += (ch-48); 
								}
								sz *= ds ; 
								flag = true;
						}
						
						if(words[i][j+1].find(',')>0 && words[i][j+1].find(',')<99999){
		
							arrayFlag = true;
							int arraySize = records[symTabCounter].checkSize(words[i][j]);
							for(int x=0 ; x<words[i][j+1].size() ; x++){
								char ch = words[i][j+1].at(x);
								if(ch == ',')
									sz+=arraySize;
							}
						}
						records[symTabCounter].setName(words[i][j-1]);
						records[symTabCounter].setSize(records[symTabCounter].checkSize(words[i][j]));
						records[symTabCounter].setSegment(records[symTabCounter].checkSegment(words[i][j],i,j));
						records[symTabCounter].setType(records[symTabCounter].checkType(words[i][j]));
						records[symTabCounter].setOffset(records[symTabCounter].checkOffset(words[i][j],sz,flag,arrayFlag));
						
						symTabCounter++;
					}
					
//			if(wordID=="ASSUME"){
//						;
//			}

			
			else if(wordID=="LABEL"){
				int len = words[i][j].size();
				string name = words[i][j].substr(0,len-1);
				records[symTabCounter].setName(name);
				records[symTabCounter].setSize(-1);
				records[symTabCounter].setSegment(records[symTabCounter].checkSegment(words[i][j],i,j));
				records[symTabCounter].setType("LABEL");
				records[symTabCounter].setOffset(records[symTabCounter].checkOffset("LABEL"));
					
				symTabCounter++;
					
			}
			
			else if(wordID == "OPCODE1"){
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE1"));
			}
			
			else if(wordID == "OPCODE2"){
				string value = words[i][j+1];
				string *operands;
				operands = split(value);
				bool disp = false;
				if((operands[0].find('+')>0 && operands[0].find('+')<99999) || (operands[1].find('+')>0 && operands[1].find('+')<99999))
					disp = true;
					
				if(checkOpr(operands[0]) == "REG" && checkOpr(operands[1])=="REG")
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"RTOR"));
				else if((checkOpr(operands[0]) == "REG" && checkOpr(operands[1])=="MEM") || (checkOpr(operands[1]) == "REG" && checkOpr(operands[0])=="MEM"))
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"RTOM"));
				else if((checkOpr(operands[0]) == "REG" && checkOpr(operands[1])=="MEM" && disp) || (checkOpr(operands[1]) == "REG" && checkOpr(operands[0])=="MEM" && disp))
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"RTOM_DISP",2));
				else if(checkOpr(operands[0]) == "REG" && checkOpr(operands[1])=="IMM")
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"IMMTOR"));
				else if(checkOpr(operands[0]) == "REG" && checkOpr(operands[1])=="IMM" && disp)
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"IMMTOR_DISP"));
				
					
					
/*				if((operands[0].substr(0,1)=="[" || operands[1].substr(0,1)=="[" ) && !disp) 
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"INDIRECT"));
				else if((operands[0].substr(0,1)=="[" || operands[1].substr(0,1)=="[" ) && disp)
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"INDIRECT_DISP",2));
				else 
					records[symTabCounter].setOffset(records[symTabCounter].checkOffset("OPCODE2",0,false,false,"DIRECT"));
*/			
			}
			
			j++;
		}
		i++;
	}
	
}

void buildST(){
			words= new string*[rows];
			wordsInLine = new int[rows];
			
			for(int i=0;i<rows;i++){
				words[i] = new string[5];
				for(int j=0;j<5;j++)
					words[i][j]="";
			}
			
			int i = 0,j,cw;
			while(i<rows){
				
				j = 0,cw=0;
				while(file[i][j]==' ')			//for triming whitespaces before instruction
					j++;
					
				while(file[i][j]!='\0'){		//Until we reach end of one statement we get every word
					while(file[i][j]!=' ' && file[i][j]!='\0') 
						words[i][cw]+=file[i][j++];
						
					while(file[i][j]==' ')
						j++;
						
				
				///////////////////   SYMBOL TABLE   ////////////////////
/*				    string wordID=records[symTabCounter].check(words[i][cw]);
					if(wordID=="DIRECTIVE"){			//building symbol table
					
						records[symTabCounter].setName(words[i][cw-1]);
						records[symTabCounter].setSize(records[symTabCounter].checkSize(words[i][cw]));
						records[symTabCounter].setSegment(records[symTabCounter].checkSegment(words[i][cw],i,cw));
						records[symTabCounter].setType(records[symTabCounter].checkType(words[i][cw]));
						records[symTabCounter].setOffset(records[symTabCounter].checkOffset(words[i][cw]));
						
						symTabCounter++;
					}
					
					if(wordID=="ASSUME"){
						;
					}
				*/	
				//	int len = words[i][cw].size();
				//	cout<<words[i][cw]<<" "<<records[i].check(words[i][cw])<<endl;
					cw++;
				}
				wordsInLine[i] = cw;
				i++;
			}
			buildSymbolTable();
}
		
int numOfRows(){
	ifstream infile("QUES1.TXT");
	char buffer[80];	
	while(!infile.eof()){
		infile.getline(buffer,80);
		rows++; 
	}	
}

void readFile(){
	ifstream infile("QUES1.TXT");
	char buffer[80];	
		
	numOfRows();	
	file = new char*[rows];
	
	int i=0,j;
	while(!infile.eof()){
		infile.getline(buffer,80);
		file[i] = new char[80];
		for(j=0;buffer[j]!='\0';j++)
			file[i][j]=buffer[j];
		file[i][j]='\0';
		i++;
	}	

}

void showFile(){
	for(int k=0;k<rows;k++){
		cout<<file[k]<<endl;
	}
}

int main(){
	do{
	int ch;
	cout<<"\n......MENU......"<<endl;
	cout<<" 1....Read File "<<endl;
	cout<<" 2....Build Symbol Table "<<endl;
	cout<<" 3....Exit "<<endl;
	cout<<" Enter Your Choice(1-3) "<<endl;
	cin>>ch;
	switch(ch)
	{
		case 1: readFile();
				break;
		case 2: cout<<"\n\n\n\t\t\t SYMBOL TABLE \n\n";
				buildST();
				displayST(symTabCounter);
				break;
		case 3: exit(0);
		default : cout<<"\nYou have entered wrong choice...!!!!";
	   			   break;
	}
	}while(1);
	return 0;
}
