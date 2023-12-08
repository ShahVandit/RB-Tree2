#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <ctime>
enum Color { BLACK, RED };

class ReservationNode{
	public:
		int patronID;
		int priorityNumber;
		time_t timeOfReservation;
		ReservationNode(int patronID, int priorityNumber): patronID(patronID), priorityNumber(priorityNumber){
			timeOfReservation=time(0);
		}
};

class ReservationHeap{
	int max=20;
	int size;
	public:
	std::vector<ReservationNode>reservations;
	int parent(int index) { return (index - 1) / 2; }
    int leftChild(int index) { return (2 * index + 1); }
    int rightChild(int index) { return (2 * index + 2); }
		ReservationHeap(){
			size=0;
		}
		// checking the timestamp as well as priority number
		void heapifyUp(int index){
			while((index != 0 && reservations[parent(index)].priorityNumber > reservations[index].priorityNumber) ||
				reservations[parent(index)].priorityNumber==reservations[index].priorityNumber && reservations[parent(index)].timeOfReservation>reservations[index].timeOfReservation){
            std::swap(reservations[parent(index)], reservations[index]);
            index = parent(index);
        	}
		}
		void heapifyDown(int index) {
        int left = leftChild(index);
        int right = rightChild(index);
        int smallest = index;

        if (left < reservations.size() && reservations[left].priorityNumber < reservations[index].priorityNumber)
            smallest = left;

        if (right < reservations.size() && reservations[right].priorityNumber < reservations[smallest].priorityNumber)
            smallest = right;

        if (smallest != index) {
            std::swap(reservations[index], reservations[smallest]);
            heapifyDown(smallest);
        }
    }

};
class BookNode {
    public:
    int bookId;
    std::string bookName;
    std::string authorName;
    std::string availabilityStatus;
    int borrowedBy;
    ReservationHeap rhp;
    Color color;

    BookNode* parent;
    BookNode* left;
    BookNode* right;

    BookNode(int id, std::string name, std::string author, std::string available)
    : bookId(id), bookName(name), authorName(author), availabilityStatus(available),
        borrowedBy(-1),
        color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
    
};

class Library {
private:
    BookNode* root;
	int color_flip=0;

public:
    BookNode* EXTNODE;
    Library(){
    ReservationHeap rp;
    EXTNODE= new BookNode(-1, "", "", "No");
	EXTNODE->color=BLACK;
    root = EXTNODE;
    }


// fix the rb tree modified by the delete operation
	void fixDelete(BookNode* x) {
		
        while (x != root && x && x->color == BLACK) {
            if (x == x->parent->left) {
                BookNode* w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK; color_flip+=1;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (!w->right || w->right->color == BLACK) {
                        if (w->left)
                            w->left->color = BLACK;
							color_flip+=1;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right)
                        w->right->color = BLACK;
						color_flip+=1;
                    leftRotate(x->parent);
                    x = root;
                }
            } else {
                BookNode* w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
					color_flip+=1;
                }
                if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (!w->left || w->left->color == BLACK) {
                        if (w->right)
                            w->right->color = BLACK;
							color_flip+=1;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left)
                        w->left->color = BLACK;
						color_flip+=1;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        if (x)
            x->color = BLACK;
    }

	void deleteBook(int bookID) {
    BookNode* z = EXTNODE;
    BookNode* x, *y;
    BookNode* book = this->root;
	
    while (book != EXTNODE) {
        if (book->bookId == bookID) {
            z = book;
        }

        if (book->bookId <= bookID) {
            book = book->right;
        } else {
            book = book->left;
        }
    }
    if (z == EXTNODE) {
        std::cout << "Book " <<bookID<<" is no longer available "<< std::endl;
        return;
    }
    y = z;
    Color y_original_color = y->color;
    if (z->left == EXTNODE) {
        x = z->right;
        rbTransplant(z, z->right);
    } else if (z->right == EXTNODE) {
        x = z->left;
        rbTransplant(z, z->left);
    } else {
		// if degree is 2
        y = minimum(z->right);
        y_original_color = y->color;
        x = y->right;
        if (y->parent == z) {
            x->parent = y;
        } else {
            rbTransplant(y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        rbTransplant(z, y);
        y->left = z->left;
        y->left->parent = y;
        y->color = z->color; 
    }

    if (y_original_color == BLACK) {
		fixDelete(x);
	}
	std::string msg= "Book "+ std::to_string(z->bookId) +" is no longer available.";
	if(!z->rhp.reservations.empty()){
		std::string temp;
		if(z->rhp.reservations.size()==1) temp="Reservations made by Patron";
		else temp="Reservations made by Patrons";
		
		for(auto num:z->rhp.reservations){
			temp+=" "+std::to_string(num.patronID)+",";
		}
		temp.pop_back();
		temp+=" have been cancelled!";
		msg+=temp;
	}
	std::cout<<msg<<std::endl;
	EXTNODE->color=BLACK;
}

void rbTransplant(BookNode* u, BookNode* v){
    if (u->parent == nullptr) {
        root = v;
        root->color = u->color;  // Preserve the color of the root node
    } else if (u == u->parent->left) {
        u->parent->left = v;
        if (v != EXTNODE) {
            v->color = u->color;  // Preserve the color of the replaced node
			// color_flip+=1;
        }
    } else {
        u->parent->right = v;
        if (v != EXTNODE) {
            v->color = u->color;  // Preserve the color of the replaced node
			color_flip+=1;
        }
    }
    v->parent = u->parent;
}
	BookNode* minimum(BookNode* node) {
		while (node->left != EXTNODE) {
			node = node->left;
		}
		return node;
	}
	void insertBook(int id, std::string name, std::string author, std::string available){
		BookNode* book= new BookNode(id, name, author, available);
		insert(book);
		// insert()
	}
    void insert(BookNode* book) {
		// Ordinary Binary Search Insertion
		book->left = EXTNODE; 
		book->parent = nullptr;
		book->right = EXTNODE;
		BookNode* y = nullptr;
		BookNode* x = this->root;
		// std::cout<<"color "<<EXTNODE->color<<std::endl;
		// std::cout<<"ID "<<book->bookId<<std::endl;

		while (x != EXTNODE) {
			y = x;
			if (book->bookId < x->bookId) {
				x = x->left;
			} else {
				x = x->right;
			}
		}

		// y is parent of x
		book->parent = y;
		if (y == nullptr) {
			this->root = book;
		} else if (book->bookId < y->bookId) {
			y->left = book;
		} else {
			y->right = book;
		}

		// if new node is a root node, simply return
		if (book->parent == nullptr){
			book->color = BLACK;
			return;
		}

		// if the grandparent is null, simply return
		if (book->parent->parent == nullptr) {
			return;
		}

		// Fix the tree
		fixInsert(book);
	}

    void fixInsert(BookNode* k){
		BookNode* u;
        while (k->parent->color == RED) { // Red - Red conflict
			if (k->parent == k->parent->parent->right) { //if k's parent is right child
				u = k->parent->parent->left; // uncle
				if (u->color == RED) {
					// case 3.1
					u->color = BLACK;
					k->parent->color = BLACK;
					color_flip += 2; // Two color flips occurred
					if(k->parent->parent->color==BLACK && k->parent->parent!=root) color_flip+=1;
					k->parent->parent->color = RED;
					k = k->parent->parent;
					color_flip+=1;
				} else {
					if (k == k->parent->left) {
						// case 3.2.2
						k = k->parent;
						rightRotate(k);
					}
					// case 3.2.1
					k->parent->color = BLACK;
					k->parent->parent->color = RED;
					leftRotate(k->parent->parent);
					color_flip+=1;
					color_flip += 2; // Two color flips occurred
				}
			} else { // if k's parent is left child
				u = k->parent->parent->right; // uncle

				if (u->color == RED) {
					// mirror case 3.1
					u->color = BLACK;
					k->parent->color = BLACK; 
					color_flip+=2;
					if(k->parent->parent->color==BLACK && k->parent->parent!=root)
					{
						color_flip+=1;
						k->parent->parent->color = RED;
					} 
					k = k->parent->parent;
					// color_flip+=1;
				} else {
					if (k == k->parent->right) {
						// mirror case 3.2.2
						k = k->parent;
						leftRotate(k);
					}
					// mirror case 3.2.1
					k->parent->color = BLACK;
					k->parent->parent->color = RED;
					rightRotate(k->parent->parent);
					// color_flip+=1;
					color_flip += 2; // Two color flips occurred
				}
			}
			if (k == root) {
				break;
			}
   		}
		// if(root->color==RED) color_flip+=1;
		root->color = BLACK;
		}
    
	// rotate left at node x
	void leftRotate(BookNode* x) {
		BookNode* y = x->right;
		x->right = y->left;
		if (y->left != EXTNODE) {
			y->left->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		} else if (x == x->parent->left) {
			x->parent->left = y;
		} else {
			x->parent->right = y;
		}
		y->left = x;
		x->parent = y;
	}

	// rotate right at node x
	void rightRotate(BookNode* x) {
		BookNode* y = x->left;
		x->left = y->right;
		if (y->right != EXTNODE) {
			y->right->parent = x;
		}
		y->parent = x->parent;
		if (x->parent == nullptr) {
			this->root = y;
		} else if (x == x->parent->right) {
			x->parent->right = y;
		} else {
			x->parent->left = y;
		}
		y->right = x;
		x->parent = y;
	}

	void borrowBook(int patronID, int bookID, int priorityNumber){
		BookNode* book=searchBook(bookID);
		if(book->rhp.reservations.empty() && book->availabilityStatus=="Yes"){
			std::cout<<"Book "<<bookID<<" borrowed by "<<patronID<<std::endl;
			book->availabilityStatus="No";
			book->borrowedBy=patronID;
		} else if(book->rhp.reservations.size()==20){
			std::cout<<"Reservation list is already full";
			return;
		} else{
			book->rhp.reservations.push_back(ReservationNode(patronID,priorityNumber));
			int index=book->rhp.reservations.size()-1;
			book->rhp.heapifyUp(index);
			std::cout<<"Book "<<bookID<<" reserved by Patron "<<patronID<<std::endl;
			// std::cout<<book->rhp.reservations.size();
		}
	}
	void returnBook(int patronID, int bookID) {
		BookNode* book=searchBook(bookID);
		if(book->rhp.reservations.empty()){
			std::cout<<"Book "<<bookID<<" returned by patron "<<patronID<<std::endl;
			book->availabilityStatus="Yes";
			return;
		}
		int c = book->rhp.reservations[0].patronID;
		book->borrowedBy=book->rhp.reservations[0].patronID;
		book->rhp.reservations[0] = book->rhp.reservations.back();
		// ReservationNode c=book->rhp.reservations[0];
        book->rhp.reservations.pop_back();
        book->rhp.heapifyDown(0);
		if(book->rhp.reservations.empty()) book->availabilityStatus="Yes";
		std::cout<<"Book "<<bookID<<" returned by "<<patronID<<std::endl;
		std::cout<<"Book "<<bookID<<" alloted to "<<c<<std::endl;
		// if(book->rhp.reservations.empty())
	
    }
	BookNode* searchBook(int bookID){
		return searchTreeHelper(this->root, bookID);
	}

    void printBook(int bookID) {
		BookNode* book= searchTreeHelper(this->root, bookID);
		if(book->bookId!=-1){
		std::cout<<"BookID"<<" = "<< book->bookId<<std::endl;
		std::cout<<"Title"<<" = "<< book->bookName<<std::endl;
		std::cout<<"Author"<<" = "<< book->authorName<<std::endl;
		std::cout<<"Availability"<<" = "<< book->availabilityStatus<<std::endl;
		if(book->borrowedBy!=-1) std::cout<<"BorrowedBy"<<" = "<< book->borrowedBy<<std::endl;
		std::string temp;
		if(!book->rhp.reservations.empty()){
			temp+="[";
		for(auto num:book->rhp.reservations){
			temp+=std::to_string(num.patronID);
			temp+=",";
		}
		temp.pop_back();
		temp+="]";
		std::cout<<"Reservations"<<" = "<<temp<<std::endl;
		} else{
		std::cout<<"Reservations"<<" = []"<<std::endl;
		}
		} else{
			std::cout<<"Book "<<bookID<<" not found in the library"<<std::endl;
		}
		std::cout<<"\n";
	}
	void printBooks(int bookID1, int bookID2) {
		printRangeHelper(this->root, bookID1, bookID2);
		}

	void printRangeHelper(BookNode* book, int bookID1, int bookID2){
		if (book == EXTNODE) {
			return;
		}

		printRangeHelper(book->left, bookID1, bookID2);
		if(book->bookId>bookID1 && book->bookId<bookID2) printBook(book->bookId);
		printRangeHelper(book->right, bookID1, bookID2);

	}
    BookNode* searchTreeHelper(BookNode* book, int bookID) {
		if (book == EXTNODE || bookID == book->bookId) {
			return book;
		}

		if (bookID < book->bookId) {
			return searchTreeHelper(book->left, bookID);
		}
		return searchTreeHelper(book->right, bookID);
	}
	void closestBook(int n){
	int diff = 0;
    std::vector<int> result;
    while (result.empty()) {
        searchClosest(this->root, result, n, diff);
        diff++;
    }

	for(auto num:result){
		printBook(num);
	}
	}

	void searchClosest(BookNode* node, std::vector<int> &result, int key,int diff) {
		if(node==EXTNODE){
			return;
		}
		if (diff == abs(node->bookId-key)) {
			result.push_back(node->bookId);
		}

		if (key < node->bookId) {
			return searchClosest(node->left, result, key, diff);
		} else{
			return searchClosest(node->right, result, key, diff);
		} 
	}

	void get_color_flips(){
		std::cout<<"Color flips "<<color_flip<<std::endl;
	}
	void prettyPrint() {
	    if (root) {
    		printHelper(this->root, "", true);
	    }
	}
	void printHelper(BookNode* root, std::string indent, bool last) {
		// print the tree structure on the screen
	   	if (root != EXTNODE) {
		   std::cout<<indent;
		   if (last) {
		      std::cout<<"R----";
		      indent += "     ";
		   } else {
		      std::cout<<"L----";
		      indent += "|    ";
		   }
            
           Color sColor = root->color;
		   std::cout<<root->bookId<<"("<<sColor<<")"<<std::endl;
		   printHelper(root->left, indent, false);
		   printHelper(root->right, indent, true);
		}
		// cout<<root->left->data<<endl;
	}
};

void executeCommand(const std::string& commandLine, Library& lib) {
	std::smatch matches;
    
    // Regex patterns for different commands
    std::regex insertBookRegex("InsertBook\\((\\d+),\\s*\"([^\"]+)\",\\s*\"([^\"]+)\",\\s*\"([^\"]+)\"\\)");
    std::regex printBookRegex("PrintBook\\((\\d+)\\)");
    std::regex printBooksRegex("PrintBooks\\((\\d+),\\s*(\\d+)\\)");
    std::regex borrowBookRegex("BorrowBook\\((\\d+),\\s*(\\d+),\\s*(\\d+)\\)");
    std::regex returnBookRegex("ReturnBook\\((\\d+),\\s*(\\d+)\\)");
    std::regex findClosestBookRegex("FindClosestBook\\((\\d+)\\)");
    std::regex deleteBookRegex("DeleteBook\\((\\d+)\\)");
    std::regex colorFlipCountRegex("ColorFlipCount\\(\\)");

    if (std::regex_match(commandLine, matches, insertBookRegex)) {
        int id = std::stoi(matches[1].str());
        std::string title = matches[2].str();
        std::string author = matches[3].str();
        std::string available = matches[4].str();

        lib.insertBook(id, title, author, available);
    } else if (std::regex_match(commandLine, matches, printBookRegex)) {
        int id = std::stoi(matches[1].str());
        lib.printBook(id);
    } else if (std::regex_match(commandLine, matches, printBooksRegex)) {
        int id1 = std::stoi(matches[1].str());
        int id2 = std::stoi(matches[2].str());
        lib.printBooks(id1, id2);
    } else if (std::regex_match(commandLine, matches, borrowBookRegex)) {
        int userId = std::stoi(matches[1].str());
        int bookId = std::stoi(matches[2].str());
        int duration = std::stoi(matches[3].str());

        lib.borrowBook(userId, bookId, duration);
    } else if (std::regex_match(commandLine, matches, returnBookRegex)) {
        int userId = std::stoi(matches[1].str());
        int bookId = std::stoi(matches[2].str());

        lib.returnBook(userId, bookId);
    } else if (std::regex_match(commandLine, matches, findClosestBookRegex)) {
        int bookId = std::stoi(matches[1].str());
        lib.closestBook(bookId);
    } else if (std::regex_match(commandLine, matches, deleteBookRegex)) {
        int bookId = std::stoi(matches[1].str());
        lib.deleteBook(bookId);
    } else if (std::regex_match(commandLine, matches, colorFlipCountRegex)) {
        lib.get_color_flips();
    } else{
		std::cout<<"Error in command "<<commandLine<<std::endl;
	}
	// else if(commandLine=="Quit()"){
	// }
}

int main(){
    Library lib;
	std::ifstream file("input.txt");
    std::string line;

    if (file.is_open()) {
        while (getline(file, line)) {
			if(line=="Quit()"){
			std::cout<<"Program Terminated!!"<<std::endl;
			return 0;
			}
			while(line.back()!=')') line.pop_back();
			// std::cout<<line<<std::endl;
            executeCommand(line, lib);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file";
    }
    // lib.insertBook(101, "Introduction to Algorithms", "Thomas H. Cormen","Yes");
    // lib.insertBook(48, "Data Structures and Algorithms", "Sartaj Sahni","Yes");
	// // lib.prettyPrint();
	// lib.printBook(48);
    // lib.insertBook(132, "Operating System Concepts", "Abraham Silberschatz","Yes");
    // lib.insertBook(25, "Computer Networks", "Andrew S. Tanenbaum", "Yes");
	// lib.borrowBook(120, 48, 2);
	// lib.borrowBook(132, 101, 1);
    // lib.insertBook(73, "Introduction to the Theory of Computation", "Michael Sipser","Yes");
    // lib.insertBook(12, "Artificial Intelligence: A Modern Approach", "Stuart Russell","Yes");
    // lib.insertBook(6, "Database Management Systems", "Raghu Ramakrishnan","Yes");
	// lib.borrowBook(144,48,3);
	// lib.borrowBook(140,48,3);
	// lib.borrowBook(142,48,2);
	// lib.borrowBook(138,12,4);
	// lib.borrowBook(150,12,3);
	// lib.borrowBook(162,12,1);
	// lib.returnBook(120, 48);
	// lib.closestBook(9);
	// lib.deleteBook(12);
	// lib.get_color_flips();
    // lib.insertBook(125, "Computer Organization and Design", "David A. Patterson","Yes");
    // lib.insertBook(180, "Introduction to Software Engineering", "Ian Sommerville","Yes");
	// lib.borrowBook(111, 73, 3);
	// lib.borrowBook(52, 73, 1);
    // lib.insertBook(115, "Operating Systems: Internals and Design Principles","William Stallings","Yes");
	// lib.borrowBook(153, 25, 2);
	// lib.printBooks(10, 150);
	// lib.insertBook(210, "Machine Learning: A Probabilistic Perspective","Kevin P. Murphy", "Yes");
	// lib.borrowBook(171, 25, 3);
	// lib.borrowBook(2, 132, 2);
	// lib.closestBook(50);
	// lib.borrowBook(18, 101, 2);
	// lib.insertBook(80, "Software Engineering: A Practitioner's Approach","Roger S. Pressman", "Yes");
	// lib.borrowBook(210, 210, 1);
	// lib.borrowBook(43, 73, 1);
	// lib.insertBook(60, "Introduction to Computer Graphics", "David F. Rogers","Yes");
	// lib.printBook(210);
	// lib.insertBook(4,"Design PaBook 5 borrowed by 101tterns: Elements of Reusable Object-Oriented Software", "Erich Gamma", "Yes");
	// lib.insertBook(2, "Introduction to the Theory of Computation", "Michael Sipser", "Yes");
	// lib.borrowBook(34, 210, 2);
	// lib.insertBook(65,"Computer Networks: Principles, Protocols, and Practice", "Olivier Bonaventure", "Yes");
	// lib.get_color_flips();
	// lib.deleteBook(125);
	// lib.deleteBook(115);
	// lib.deleteBook(210);
	// lib.get_color_flips();
	// lib.deleteBook(25);
	// lib.deleteBook(80);
	// lib.get_color_flips();

    return 0;
}