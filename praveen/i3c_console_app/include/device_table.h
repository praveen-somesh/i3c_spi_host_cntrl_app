#include<stdio.h>
#include<stdlib.h>
FILE *master_CmdResp_FileObj = NULL;
FILE *master_IbiResp_FileObj = NULL;
FILE *slave_CmdResp_FileObj = NULL;
typedef unsigned char uint8;
static int index=0;
struct node{
    uint8 pid5;
    uint8 pid4;
    uint8 pid3;
    uint8 pid2;
    uint8 pid1;
    uint8 pid0;
    uint8 bcr;
    uint8 dcr;
    uint8 dynamic_addr;
    uint8 static_addr;
    struct node *next;
};
struct node *head;
void create_slave_node
(uint8 pid5,
uint8 pid4,
uint8 pid3,
uint8 pid2,
uint8 pid1,
uint8 pid0,
uint8 bcr,
uint8 dcr,
uint8 dynamic_addr,
uint8 static_addr)
{
							//inserting at begin
    struct node *temp;
    temp = (struct node*)malloc(sizeof(struct node));
    temp->pid5 = pid5;
    temp->pid4 = pid4;
    temp->pid3 = pid3;
    temp->pid2 = pid2;
    temp->pid1 = pid1;
    temp->pid0 = pid0;
    temp->bcr = bcr;
    temp->dcr = dcr;
    temp->dynamic_addr = dynamic_addr;
    temp->static_addr = static_addr;
    temp->next = head;
    head= temp;
}
void create_slave_node_end
(uint8 pid5,
uint8 pid4,
uint8 pid3,
uint8 pid2,
uint8 pid1,
uint8 pid0,
uint8 bcr,
uint8 dcr,
uint8 dynamic_addr,
uint8 static_addr)
{
							//inserting at begin
    struct node *newNode,*temp;
    newNode = (struct node*)malloc(sizeof(struct node));
	if(newNode == NULL)
    {
        printf("Unable to allocate memory for DCT\n");
		return;
    }
     else
    {
		temp = head;
		
        newNode->pid5 = pid5;
		newNode->pid4 = pid4;
		newNode->pid3 = pid3;
		newNode->pid2 = pid2;
		newNode->pid1 = pid1;
		newNode->pid0 = pid0;
		newNode->bcr = bcr;
		newNode->dcr = dcr;
		newNode->dynamic_addr = dynamic_addr;
		newNode->static_addr = static_addr;
		
        newNode->next = NULL; 

		if(head==NULL)
		{
			head=newNode;
			temp=newNode;
		}
		else
		{
			//Traverse to the last node
			while(temp->next!= NULL)
			{
		
				temp = temp->next;
			}
			temp->next = newNode; //Links the address part
        }
		//printf("DATA INSERTED SUCCESSFULLY\n");
    }
}
int change_dynamic_addr(uint8 old_addr,uint8 new_addr)
{
	int pos=0;
	struct node *temp=NULL;
	
	if(head==NULL){
		printf("\tDCT has no Data to Modify\n");
		return 0;
	}
	temp=head;
	while(temp->next!=NULL)
	{
		if(temp->dynamic_addr==old_addr)
		{
			temp->dynamic_addr=new_addr;
			// printf("\tFound at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found dynamic address entry in DCT at position :%d",pos);
			return 1;
		}
		pos++;
		temp=temp->next;
		if(temp->next== NULL)
		{
			if(temp->dynamic_addr==old_addr)
			{
			temp->dynamic_addr=new_addr;
			// printf("\tDyna_Addr Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found dynamic address entry in DCT at position :%d",pos);
			return 1;
			}
		}
	}
	// printf("\t dynamic_addr 0X:%02x not Found in DCT",old_addr);
	fprintf(master_CmdResp_FileObj,"dynamic_addr 0x:%02x not found in DCT",old_addr);
			return 1;
	return 0;
}
int change_pid(uint8 old_addr,uint8 *pid)
{
	int pos=0;
	struct node *temp=NULL;
	
	if(head==NULL){
		printf("\tDCT has no Data to Modify\n");
		return 0;
	}
	temp=head;
	while(temp->next!=NULL)
	{
		if(temp->dynamic_addr==old_addr)
		{
			temp->pid5 = pid[0];
			temp->pid4 = pid[1];
			temp->pid3 = pid[2];
			temp->pid2 = pid[3];
			temp->pid1 = pid[4];
			temp->pid0 = pid[5];
			// printf("\t PID Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found PID entry in DCT at position :%d",pos);
			return 1;
		}
		pos++;
		temp=temp->next;
		if(temp->next== NULL)
		{
			if(temp->dynamic_addr==old_addr)
			{
			temp->pid5 = pid[0];
			temp->pid4 = pid[1];
			temp->pid3 = pid[2];
			temp->pid2 = pid[3];
			temp->pid1 = pid[4];
			temp->pid0 = pid[5];
			// printf("\t PID Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found PID entry in DCT at position :%d",pos);
			return 1;
			}
		}
	}
	return 0;
}
int change_bcr(uint8 old_addr,uint8 bcr)
{
	int pos=0;
	struct node *temp=NULL;
	
	if(head==NULL){
		printf("\tDCT has no data to modify\n");
		return 0;
	}
	temp=head;
	while(temp->next!=NULL)
	{
		if(temp->dynamic_addr==old_addr)
		{
			temp->bcr = bcr;
			// printf("\t BCR Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found BCR entry in DCT at position :%d",pos);
			return 1;
		}
		pos++;
		temp=temp->next;
		if(temp->next== NULL)
		{
			if(temp->dynamic_addr==old_addr)
			{
			temp->bcr = bcr;
			// printf("\t BCR Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found BCR entry in DCT at position :%d",pos);
			return 1;
			}
		}
	}
	return 0;
}
int change_dcr(uint8 old_addr,uint8 dcr)
{
	int pos=0;
	struct node *temp=NULL;
	
	if(head==NULL){
		printf("\tDCT has no Data to Modify\n");
		return 0;
	}
	temp=head;
	while(temp->next!=NULL)
	{
		if(temp->dynamic_addr==old_addr)
		{
			temp->dcr = dcr;
			// printf("\t DCR Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found DCR entry in DCT at position :%d",pos);
			return 1;
		}
		pos++;
		temp=temp->next;
		if(temp->next== NULL)
		{
			if(temp->dynamic_addr==old_addr)
			{
			temp->dcr = dcr;
			// printf("\t DCR Found at position :%d",pos);
			fprintf(master_CmdResp_FileObj,"Found DCR entry in DCT at position :%d",pos);
			return 1;
			}
		}
	
	}
	return 0;
}
int reset_dct()
{
printf("\tDeleting the DCT \n");
head=NULL;
printf("\tDONE \n");
fprintf(master_CmdResp_FileObj,"DCT deleted.\n");
return 0;
}
int delete(uint8 num)
{
    struct node *temp, *prev;
    temp=head;
    while(temp!=NULL)
    {
    if(temp->dynamic_addr==num)
    {
        if(temp==head)
        {
        head=temp->next;
        free(temp);
        return 1;
        }
        else
        {
        prev->next=temp->next;
        free(temp);
        return 1;
        }
    }
    else
    {
        prev=temp;
        temp= temp->next;
    }
    }
    return 0;
}
void print_dct()
{
    struct node *temp;
    temp= head;
    //printf("\n the linked list is :\n");
	index=0;
	if(temp==NULL)
	{
	printf("\n\t------DCT has NO Data to Display------\n");
	return;
	}
	printf("\n\n  -----------------------------------------------------------------------\n");
	fprintf(master_CmdResp_FileObj,"\n\n  -----------------------------------------------------------------------\n");
	printf("  _____________________________   I3C   _________________________________\n");
	fprintf(master_CmdResp_FileObj,"  _____________________________   I3C   _________________________________\n");
	printf("  _________________   Device characteristics table   ____________________\n");
	fprintf(master_CmdResp_FileObj,"  _________________   Device characteristics table   ____________________\n");
	printf("  -----------------------------------------------------------------------\n");
	printf("\n  | S.N.\t| Dynamic Addr.\t| PID\t\t\t| BCR\t| DCR\t|\n");
	fprintf(master_CmdResp_FileObj,"\n  | S.N.\t| Dynamic Addr.\t| PID\t\t\t| BCR\t| DCR\t|\n");
	printf("  -----------------------------------------------------------------------\n");
	fprintf(master_CmdResp_FileObj,"  -----------------------------------------------------------------------\n");
    while(temp!=NULL){
		printf("  | %02d\t\t| 7'h%02x\t\t| 48'h%02x%02x%02x%02x%02x%02x\t| 8'h%02x\t| 8'h%02x\t|\n",
		++index,
		temp->dynamic_addr,
		temp->pid5,
		temp->pid4,
		temp->pid3,
		temp->pid2,
		temp->pid1,
		temp->pid0,
		temp->bcr,
		temp->dcr
		);
		
		
		fprintf(master_CmdResp_FileObj,"  | %02d\t\t| 7'h%02x\t\t| 48'h%02x%02x%02x%02x%02x%02x\t| 8'h%02x\t| 8'h%02x\t|\n",
		index,
		temp->dynamic_addr,
		temp->pid5,
		temp->pid4,
		temp->pid3,
		temp->pid2,
		temp->pid1,
		temp->pid0,
		temp->bcr,
		temp->dcr
		);
		temp=temp->next;
						
		fprintf(master_CmdResp_FileObj,"\n");	
	}
	printf("  -----------------------------------------------------------------------\n\n");
}                              
void print_dat(){
    struct node *temp;
    temp= head;
    //printf("\n the linked list is :\n");
	index=0;
	if(temp==NULL)
	{
	printf("\n\t------DCT has no data to display------\n");
	// fprintf(master_CmdResp_FileObj,"\n\t------DCT has no data to display------\n");
	return;
	}
	printf("\n\n  -----------------------------------------------------------------------\n");
	// fprintf(master_CmdResp_FileObj,"\n\n  -----------------------------------------------------------------------\n");
	printf("  _____________________________   I3C   _________________________________\n");
	// fprintf(master_CmdResp_FileObj,"  _____________________________   I3C   _________________________________\n");
	printf("  _________________       Device Address table       ____________________\n");
	// fprintf(master_CmdResp_FileObj,"  _________________       Device Address table       ____________________\n");
	printf("  -----------------------------------------------------------------------\n");
	// fprintf(master_CmdResp_FileObj,"  -----------------------------------------------------------------------\n");
	printf("\n  | S.N.\t| Dynamic Addr.\t|  BCR\t| DCR\t| | static Addr.\t|\n");
	// fprintf(master_CmdResp_FileObj,"\n  | S.N.\t| Dynamic Addr.\t|  BCR\t| DCR\t| | static Addr.\t|\n");
	printf("  -----------------------------------------------------------------------\n");
	// fprintf(master_CmdResp_FileObj,"  -----------------------------------------------------------------------\n");
    while(temp!=NULL){
	printf("  | %02d\t\t| 7'h%02x\t\t|  8'h%02x\t| 8'h%02x\t| | 7'h%02x\t|\n",
	++index,
	temp->dynamic_addr,
	temp->bcr,
	temp->dcr,
	temp->static_addr
	);
	
	fprintf(master_CmdResp_FileObj,"  | %02d\t\t| 7'h%02x\t\t|  8'h%02x\t| 8'h%02x\t| | 7'h%02x\t\t|\n",
	index,
	temp->dynamic_addr,
	temp->bcr,
	temp->dcr,
	temp->static_addr
	);
	temp=temp->next;
    }                             
	printf("  -----------------------------------------------------------------------\n\n");
	// fprintf(master_CmdResp_FileObj,"  -----------------------------------------------------------------------\n\n");
}