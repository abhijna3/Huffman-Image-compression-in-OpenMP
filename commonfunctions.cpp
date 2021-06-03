
#include "headerf.h"
#include <iostream>
#include <vector>
#include <fstream>

FILE * txtfile;

struct node* buildtree(struct linklist *listhead, struct node* head){
	struct linklist *list = listhead;
	struct linklist *tmplist= NULL, *newlist=NULL; 
	while(list->next != NULL){
		struct node* tmp = (struct node*)malloc(sizeof(struct node));
		tmp -> left = list->data;
		tmp -> right = list->next->data;
		tmp -> freq = list->data->freq + list->next->data->freq;
		head = tmp;
		//insert this head in sorted list
		tmplist = list->next;
		newlist = list;
		newlist->data = head;
		list = list->next;
		//tmplist = list->next;
		while( tmplist->next!= NULL){
			if(tmplist->next->data->freq > head->freq)
				break;
			tmplist = tmplist->next;
		}
		newlist->next = tmplist -> next;
		tmplist-> next = newlist;
		tmplist = list->next;	
		free(list);
		list = tmplist;
	}
	//printf("done building tree");
	free(newlist);
	return head;
}

void Destroylist(struct linklist *start)	// free linklist
{
        struct linklist *p, *pnext;
        p=start;
		//printf("freeing memory\n");
        while(p!=NULL){
        	printf("freeing memory\n");
                pnext = p->next;
                free(p);
                p=pnext;
        }
        start = NULL;
}
void destroytree(struct node *head){	// free huffman tree
	if(head->left->left != NULL){
		destroytree(head->left);
	}
	if(head->right->right != NULL){
		destroytree(head->right);
	}
	free(head);
}
int fib(int n){		// get fibonacci number
	if(n<=1)
		return n;
	return fib(n-1)+fib(n-2);
}
void updatecode2(struct node *root, unsigned char *bitSequence, unsigned int bitSequenceLength){

	if (root->left){
		bitSequence[bitSequenceLength] = '0';
		updatecode2(root->left, bitSequence, bitSequenceLength + 1);
	}

	if (root->right){
		bitSequence[bitSequenceLength] = '1';
		updatecode2(root->right, bitSequence, bitSequenceLength + 1);
	}
    #ifdef printtree
        root->codelen = bitSequenceLength;
		memcpy(root->code , bitSequence, bitSequenceLength * sizeof(unsigned char));
    #else
	if (root->left == NULL && root->right == NULL){
		root->codelen = bitSequenceLength;
		//strcpy(root->code, bitSequence);	//, bitSequenceLength * sizeof(unsigned char));
		memcpy(root->code , bitSequence, bitSequenceLength * sizeof(unsigned char));
	}
    #endif
}

void printBT(const std::string& prefix, const node* head, bool isLeft)	//"",headtree,false,
{
    if( head != NULL )
    {
		std::fprintf(txtfile,"%s", (prefix).c_str());
		std::fprintf(txtfile,(isLeft ? "├──" : "└──" ) );
		if(head->left == NULL && head->right == NULL)
			std::fprintf(txtfile, "[%d]",head->name);					// write name of node
		
		std::fprintf(txtfile, "  f:%d  code:",head->freq); 		// write frequency of node
        
		std::fwrite(head->code, head->codelen, sizeof(unsigned char),txtfile);	// write encoded bits of node
		
		std::fprintf(txtfile,"\n");

        printBT( prefix + (isLeft ? "│   " : "    "), head->left, true);
        printBT( prefix + (isLeft ? "│   " : "    "), head->right, false);
    }
}
void printgraph(node* head){
	txtfile = fopen("huffmanTree.txt", "wb");
    // if (txtfile.fail())
    //{
     //   std::cout << "Failed to open outputfile.\n";
    //}
	printBT("", head, false);
	for(int p=0;p<maxsymbol;p++){
		if(dict[p].freq){
			std::fprintf(txtfile,"name=%d frequency=%d code=",dict[p].name,dict[p].freq );
			std::fwrite(dict[p].code, dict[p].codelen, sizeof(unsigned char),txtfile);	// write encoded bits of node
			std::fprintf(txtfile,"\n");
		}
	}

	fclose(txtfile);
}