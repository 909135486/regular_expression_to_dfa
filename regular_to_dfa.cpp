#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string>
#include<stack>
#include<map>
#define wrong_expression(ex)    if(ex){printf("wrong expression");return 0;}
#define max_edge_of_a_node  5
#define null_char   '$'  
using namespace std;
/*
this is a project for changing a regular expression to Determine the finite automaton(DFA) WITHOUT SIMPLIFICATION
the terminator for a single regular expression must be a single char from a to z,that means only 26 kinds of terminal status can be expressed
i use the $ to express eps
finaly you can get the graph of DFA ,and the value of graph[i][j] means there is an edge from node i to node j(by the way ,number 0 means nothing)
example expression :ab  a|b   a*   (a|b)*c .....
*/

struct node
{
    long name,edge_number;
    char edge[max_edge_of_a_node];
    node* next[max_edge_of_a_node];
};
struct node_index_to_address
{
    long name;
    node* address;
    node_index_to_address* next;
};

map<char,int>V_t;//Records the terminator of a regular expression
long number=0;//Number each node and record the number of nodes in the NFA
node_index_to_address* ita1=(node_index_to_address*)malloc(sizeof(node_index_to_address));
node_index_to_address* ita_end=ita1;
long number2=0;//Number each node and record the number of nodes in the DFA
node_index_to_address* ita2=(node_index_to_address*)malloc(sizeof(node_index_to_address));
stack<long>dfa_start_status,dfa_end_status;//each of them contain the start status and final status of DFA,(each DFA node named by a <long tpye> number which start from 0)


string add_add(string re){
    int len=re.length();
    for(int i=0;i<len;i++){
        if(i>0 && (re[i-1]>='a'&&re[i-1]<='z') && (re[i]>='a'&&re[i]<='z')){re.insert(i,"+");i++;len++;}
        else if(i>=0&& re[i-1]==')' && (re[i]>='a'&&re[i]<='z')){re.insert(i,"+");i++;len++;}
        else if(i>=0&& re[i]=='(' && (re[i-1]>='a'&&re[i-1]<='z')){re.insert(i,"+");i++;len++;}
        else if(i>=0&& re[i-1]=='*' && ((re[i]>='a'&&re[i]<='z')||(re[i]=='('))  ){re.insert(i,"+");i++;len++;}

    }
    return re;
}

int cmp_op(char topc,char c){//return 1 for C priority, need to be put on the stack, 0 means stack top priority
    if(topc=='(')return 1;
    else if(c=='*'&&topc!='*')return 1;
    else return 0;
}


string RPN(string re){
    re=add_add(re);
    string result="";
    stack<char>op;
    int len=re.length();
    for(int i=0;i<len;i++){
        if(re[i]>='a'&&re[i]<='z')result=result+re[i];
        else if(re[i]=='('){
            op.push(re[i]);
        }
        else if(re[i]==')'){
            while (!op.empty())
            {
                if(op.top()!='('){
                char c=op.top();
                result=result+c;
                op.pop();
                }
                else{break;}
            }
            if(op.empty()){
                result="";
                return result;
            }
            else{op.pop();}
        }
        else{
            if(!op.empty()){
            if(cmp_op(op.top(),re[i])){
                op.push(re[i]);
            }
            else{
                while (!op.empty())
                {
                    if(!cmp_op(op.top(),re[i])){
                    char c=op.top();
                    result=result+c;
                    op.pop();
                    }
                    else
                    {
                        break;
                    }
                    
                }
                op.push(re[i]);
                }
            }
            else{
                op.push(re[i]);
            }
        }  
    }
    while(!op.empty()){
        result=result+op.top();
        op.pop();
    }
    return result;
}


int add_edge(node* from,node* to,char edge_name){
    for(int i=0;i<from->edge_number;i++){
        if(from->edge[i]==edge_name && from->next[i]==to){
            return 0;
        }
    }
    from->edge[from->edge_number]=edge_name;
    from->next[from->edge_number]=to;
    from->edge_number++;
    return 0;
}



node* new_node(long &index){
    node *s;
    s=(node*)malloc(sizeof(node));
    node_index_to_address* k=(node_index_to_address*)malloc(sizeof(node_index_to_address));
    k->name=index;
    k->address=s;
    k->next=NULL;
    ita_end->next=k;
    ita_end=ita_end->next;
    s->name=index++;
    s->edge_number=0;
    return s;
}

int rexpression_to_nfa(string re,node* &head,node* &tail){
    re=RPN(re);
    number=0;
    stack<node*>head_tail_s;
    for(int i=0;i<re.length();i++){
        if(re[i]>='a'&&re[i]<='z'){
            V_t[re[i]]=1;
            head=new_node(number);
            tail=new_node(number);
            add_edge(head,tail,re[i]);
            head_tail_s.push(head);
            head_tail_s.push(tail);
        }
        else if(re[i]=='*'){
            if(head_tail_s.empty())wrong_expression(1);
            tail=head_tail_s.top();
            head_tail_s.pop();
            head=head_tail_s.top();
            head_tail_s.pop();
            add_edge(tail,head,null_char);//use $ to express null_char
            node* Thead=new_node(number);
            node* Ttail=new_node(number);
            add_edge(Thead,head,null_char);
            add_edge(tail,Ttail,null_char);
            add_edge(Thead,Ttail,null_char);
            head_tail_s.push(Thead);
            head_tail_s.push(Ttail);
        }
        else {
            if(head_tail_s.empty())wrong_expression(1);
            node* tail2=head_tail_s.top();
            head_tail_s.pop();
            node* head2=head_tail_s.top();
            head_tail_s.pop();
            if(head_tail_s.empty())wrong_expression(1);
            tail=head_tail_s.top();
            head_tail_s.pop();
            head=head_tail_s.top();
            head_tail_s.pop();
            if(re[i]=='+'){
            add_edge(tail,head2,null_char);
            head_tail_s.push(head);
            head_tail_s.push(tail2);
            }
            else if(re[i]=='|'){
                node* Thead=new_node(number);
                node* Ttail=new_node(number);
                add_edge(Thead,head,null_char);
                add_edge(Thead,head2,null_char);
                add_edge(tail,Ttail,null_char);
                add_edge(tail2,Ttail,null_char);
                head_tail_s.push(Thead);
                head_tail_s.push(Ttail);
            }
            else{
                wrong_expression(1);
            }

        }
    }
    if(head_tail_s.empty()){
        head=NULL;
        tail=NULL;
    }
    else{
        tail=head_tail_s.top();
        head_tail_s.pop();
        head=head_tail_s.top();
        head_tail_s.pop();
        if(!head_tail_s.empty()){
            wrong_expression(1);
            head=NULL;
            tail=NULL;
        }
    }
    return 0;
}


node* name_to_node_address(long name,node_index_to_address* which_ita){
    node_index_to_address* start=which_ita;
    while (start->next!=NULL)
    {
        start=start->next;
        if(start->name==name){
            return start->address;
        }
    }
    return NULL;
}


int eps_closure(int *start,int* result){
    /*Ensure that the element values in the initial state <result> are all 0*/
    stack<node*>element;
    for(int i=0;i<number;i++){
        if(start[i]!=0){
            element.push(name_to_node_address(i,ita1));
            while (!element.empty())
            {
            node* n=element.top();
            element.pop();
            result[n->name]=1;//itself belongs to EPS set
            for(int i=0;i<n->edge_number;i++){
                if(n->edge[i]==null_char && result[n->next[i]->name]==0){
                    element.push(n->next[i]);
                    }
                }
            }
        }
    }
    return 0;
}

int move_t(int *start,char through,int* result){
    for(int j=0;j<number;j++){
        if(start[j]!=0){
            node* a_node=name_to_node_address(j,ita1);
            for(int i=0;i<a_node->edge_number;i++){
                if(a_node->edge[i]==through){
                    result[a_node->next[i]->name]=1;
                }
            }
        }

    }
    return 0;
}


int eps_closure_move_t(int* start,int* result,char from){//to find eps-closure(move(start,from)),and the results are saved in <result>
    int end[number]={0};
    move_t(start,from,end);
    eps_closure(end,result);
    return 0;
}


string element_to_string(int end[],int n){
    string s="";
    for(int i=0;i<n;i++)s+=end[i]+'0';
    return s;
}


void setvalue(int start[],string s){
    for(int i=0;i<s.length();i++){
        start[i]=s[i]-'0';
    }
}

int nfa_to_dfa(node* head,node*tail){
    if(number!=0){
        map<string,int>tab,s_to_index;
        map<int,string>dfa_node_include_nfa_node;
        stack<string>element_stack;
        string s,sub,zero="";
        node* one_dfa_node;
        for(int i=0;i<number;i++)zero+='0';
        ita_end=ita2;//Forming the node chain of DFA
        int start[number]={0};
        int end[number]={0};
        start[head->name]=1;
        eps_closure(start,end);//The end set is T0
        s=element_to_string(end,number);
        tab[s]=1;
        dfa_node_include_nfa_node[number2]=s;
        s_to_index[s]=number2;
        new_node(number2);
        element_stack.push(s);
        while (!element_stack.empty())
        {
            int index;
            s=element_stack.top();
            element_stack.pop();
            one_dfa_node=name_to_node_address(s_to_index[s],ita2);
            if(s[head->name]=='1')dfa_start_status.push(s_to_index[s]);
            if(s[tail->name]=='1')dfa_end_status.push(s_to_index[s]);
            map<char,int>::iterator iter;
            for(iter=V_t.begin();iter!=V_t.end();iter++){
                setvalue(start,s);
                setvalue(end,zero);
                eps_closure_move_t(start,end,iter->first);
                sub=element_to_string(end,number);
                if(tab[sub]){//If the tab is marked, you only need to find the node according to the sub
                    index=s_to_index[sub];
                    add_edge(one_dfa_node,name_to_node_address(index,ita2),iter->first);
                }
                else{
                    tab[sub]=1;
                    index=number2;
                    dfa_node_include_nfa_node[number2]=sub;
                    s_to_index[sub]=number2;
                    node* another_node=new_node(number2);
                    add_edge(one_dfa_node,another_node,iter->first);
                    element_stack.push(sub);
                }
            }
        }
        
    }
    return 0;
}


char** make_gragh(int  node_number,node_index_to_address *chain){
    char **graph=new char*[node_number];
    for(int i=0;i<node_number;i++){
        graph[i]=new char[node_number];
        for(int j=0;j<node_number;j++)graph[i][j]='0';
    }
    while (chain->next!=NULL)
    {
        chain=chain->next;
        node* one_node=chain->address;
        long name=one_node->name;
        for(int i=0;i<one_node->edge_number;i++){
            graph[name][one_node->next[i]->name]=one_node->edge[i];
        }
    }
    return graph;
}


void show(char** graph,int node_number){
    cout<<"start status of dfa"<<endl;
    while (!dfa_start_status.empty())
    {
        cout<<dfa_start_status.top()<<endl;
        dfa_start_status.pop();
    }
    cout<<"final status of dfa"<<endl;
    while (!dfa_end_status.empty())
    {
        cout<<dfa_end_status.top()<<endl;
        dfa_end_status.pop();
    }
    cout<<"dfa graph"<<endl;
    for(int i=0;i<number2;i++){
        for(int j=0;j<number2;j++){
            cout<<graph[i][j]<<" \0";
        }
        cout<<endl;
    }

}


int main(){
    string re="a|b";//change expression to check
    node* head;
    node* tail;
    rexpression_to_nfa(re,head,tail);
    nfa_to_dfa(head,tail);
    char **graph;
    graph=make_gragh(number2,ita2);
    show(graph,number2);
    return 0;
}


