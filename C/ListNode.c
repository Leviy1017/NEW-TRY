#include <stdio.h>
#include <stdlib.h>

    struct List{
    int data;
    struct List* next;
   
};

struct List* create(int data){
    struct List* node = (struct List*)malloc(sizeof(struct List));
    node->data = data;
    node->next = NULL;
    return node;
}

void add(struct List** head, int data){
    struct List* new_Node = create(data);
    
    if (*head == NULL){
        *head = new_Node;
        return;
    }


     struct List* temp = *head;
     while (temp->next != NULL){
    temp->next = new_Node;
        
    }
}

void del(struct List** head, int data) {
    if(*head == NULL) return;
    
    if((*head)->data == data) {
        struct List* temp = *head;
        *head = (*head)->next;
        free(temp);
        return;
    }




struct List* temp = *head;
while(temp->next != NULL){
    if(temp->next->data == data){
        struct List* to_del = temp->next;
        temp->next = temp->next->next;
        free(to_del);
        return;
        temp = temp->next;
        }
    }
}
int Find(struct List* head, int data){
    struct List* temp = head;
    while (temp != NULL){
        if (temp->data == data){
            return 1;
        }
        temp = temp->next;
        return 0;
    }
    
}

void update(struct List* head, int old_data, int new_data){
    struct List* temp = head;
    while (temp != NULL){
        if(temp->data == old_data){
            temp->data = new_data;
            return;
        }
        temp = temp->next;
    }
}

void print(struct List* head) {
    struct List* temp = head;
    while (temp != NULL) {
        printf("%d ", temp->data);
        temp = temp->next;
    }
    printf("\n");
}

int main() {
    struct List* head = NULL;
    
     
    add(&head, 1);
    add(&head, 2);
    add(&head, 3);
    printf("添加后: ");
    print(head);
    
    
    printf("查找2: %s\n", find(head, 2) ? "找到" : "没找到");
    
    
    update(head, 2, 20);
    printf("修改后: ");
    print(head);
    
    
    del(&head, 1);
    printf("删除后: ");
    print(head);
    
    return 0;
}



     

