#include<iostream>
#include<cassert>

/*
    Don't Remove this Comment !!

    Copyright ©️ (c) NokonoKotlin (okoteiyu) 2024.
    Released under the MIT license(https://opensource.org/licenses/mit-license.php)
*/
template<class type_key , class type_value>
class MyMultiSet{
    private:
    struct SplayNode{
        SplayNode *parent = nullptr;// parent node
        SplayNode *left = nullptr;// left child
        SplayNode *right = nullptr;// left child
 
        type_key Key;// sorted key
        type_value Value;// value (sorted if key is same)
 
        type_key Sum_key;// Sum of Key in Subtree
        type_value Min_val,Max_val,Sum_val;// Min,Max,Sum of Value in Subtree 

        int SubTreeSize = 1;// Size of Subtree under this node

        SplayNode(){}
        SplayNode(type_key key_ , type_value val_){
            Key = key_;
            Value = val_;
            update();
        }


        // rotate ( this node - parent ) 
        void rotate(){
            if(this->parent->parent){
                if(this->parent == this->parent->parent->left)this->parent->parent->left = this;
                else this->parent->parent->right = this; 
            }

            this->parent->eval();
            this->eval();
            
            if(this->parent->left == this){
                this->parent->left = this->right;
                if(this->right)this->right->parent = this->parent;
                this->right = this->parent;
                this->parent = this->right->parent;
                this->right->parent = this;
                this->right->update();
            }else{
                this->parent->right = this->left;
                if(this->left)this->left->parent = this->parent;
                this->left = this->parent;
                this->parent = this->left->parent;
                this->left->parent = this;
                this->left->update();
            }

            this->update();
            return;
        }

        // direction of this parent (left or right)
        int state(){
            if(this->parent == nullptr)return 0;
            this->eval();
            if(this->parent->left == this)return 1;
            else if(this->parent->right == this)return 2;
            return 0;
        }

        // bottom-up splay 
        void splay(){
            while(bool(this->parent)){
                if(this->parent->state() == 0){
                    this->rotate();
                    break;
                }
                if( this->parent->state() == this->state() )this->parent->rotate();
                else this->rotate();
                this->rotate();
            }
            this->update();
            return;
        }

        // update data member
        void update(){
            this->eval();
            this->SubTreeSize = 1;
            this->Sum_key = this->Key;
            this->Max_val = this->Sum_val = this->Min_val = this->Value;
            
            // add left child
            if(bool(this->left)){
                this->left->eval();
                this->SubTreeSize += this->left->SubTreeSize;
                if(this->left->Min_val < this->Min_val)this->Min_val = this->left->Min_val;
                if(this->left->Max_val > this->Max_val)this->Max_val = this->left->Max_val;
                this->Sum_key += this->left->Sum_key;
                this->Sum_val += this->left->Sum_val;
            }

            // add right child
            if(bool(this->right)){
                this->right->eval();
                this->SubTreeSize += this->right->SubTreeSize;
                if(this->right->Min_val < this->Min_val)this->Min_val = this->right->Min_val;
                if(this->right->Max_val > this->Max_val)this->Max_val = this->right->Max_val;
                this->Sum_key += this->right->Sum_key;
                this->Sum_val += this->right->Sum_val;
            }
 
            return;
        }

        // evaluate Lazy Evaluation
        void eval(){
            // if it's necessary , write here.
        }
    };


    /*
        1. order of node's Key if [paired_compare] is false.
        2. lexicographical order of node's (Key ,Value) if [paired_compare] is true.
    */
    constexpr bool CompareNode(SplayNode *a , SplayNode *b , bool paired_compare = false){
        a->eval();
        b->eval();
        if(!paired_compare)return a->Key <= b->Key;
        else{
            if(a->Key < b->Key)return true;
            else if(a->Key == b->Key){
                if(a->Value <= b-> Value)return true;
                else return false;
            }else return false;
        }
    }
    
    // get [index]th node pointer 
    SplayNode *get_sub(int index , SplayNode *root){
        if(root==nullptr)return root;
        SplayNode *now = root;
        while(true){
            now->eval();
            int left_size = 0;
            if(now->left != nullptr)left_size = now->left->SubTreeSize;
            if(index < left_size)now = now->left;
            else if(index > left_size){
                now = now->right;
                index -= left_size+1;
            }else break;
        }
        now->splay();
        return now;
    }
 
    // merge 2 SplayTrees 
    SplayNode *merge(SplayNode *leftRoot , SplayNode *rightRoot){
        if(leftRoot!=nullptr)leftRoot->update();
        if(rightRoot!=nullptr)rightRoot->update();
        if(bool(leftRoot ) == false)return rightRoot;
        if(bool(rightRoot) == false )return leftRoot;
        rightRoot = get_sub(0,rightRoot);
        rightRoot->left = leftRoot;
        leftRoot->parent = rightRoot;
        rightRoot->update();
        return rightRoot;
    }
    
 
    // split SplayTree at [leftnum]
    std::pair<SplayNode*,SplayNode*> split(int leftnum, SplayNode *root){
        if(leftnum<=0)return std::make_pair(nullptr , root);
        if(leftnum >= root->SubTreeSize)return std::make_pair(root, nullptr);
        root = get_sub(leftnum , root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root;
        if(bool(rightRoot))rightRoot->left = nullptr;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        leftRoot->update();
        rightRoot->update();
        return std::make_pair(leftRoot,rightRoot);
    }
 
    
    
    // remove [index]th node
    std::pair<SplayNode*,SplayNode*> Delete_sub(int index , SplayNode *root){
        if(bool(root) == false)return std::make_pair(root,root);
        root = get_sub(index,root);
        SplayNode *leftRoot = root->left;
        SplayNode *rightRoot = root->right;
        if(bool(leftRoot))leftRoot->parent = nullptr;
        if(bool(rightRoot))rightRoot->parent = nullptr;
        root->left = nullptr;
        root->right = nullptr;
        root->update();
        return std::make_pair(merge(leftRoot,rightRoot) , root );
    }
    
    
    /*
        between 2 SplayNodes [A] and [B] , we define following order.
        - if [paired_compare] is false, 
            - [A] [<] [B] represent a order of these Keys.
            - [A] [==] [B] represent these Keys are same
        - if [paired_compare] is true, 
            - [A] [<] [B] represent a lexicographical order of these (Key , Value).
            - [A] [==] [B] represent these (Key , Value) are same

        This function finds the border index [B] which satisfies following.
        1. if [lower] is true, for any [i] smaller than [B] , {[i]th node} [<] {[Node] argument}
        2. if [lower] is false, for any [i] smaller than [B] , {[i]th node} [<] {[Node] argument} or  {[i]th node} [==] {[Node] argument}
    */
    std::pair<SplayNode*,int> bound_sub(SplayNode* Node , SplayNode *root , bool lower ,  bool paired_compare ){
        if(bool(root) == false)return std::make_pair(root,0);
        SplayNode *now = root;
        int res = 0;
        Node->update();
        while(true){
            now->eval();
            bool satisfy = CompareNode(now,Node,paired_compare); // upper_bound (now <= Node)
            if(lower)satisfy = !CompareNode(Node,now,paired_compare); // lower_bound (now < Node)
            if(satisfy){
                if(bool(now->right))now = now->right;
                else {
                    res++;
                    break;
                }
            }else{
                if(bool(now->left))now = now->left;
                else break;
            }
        }
        now->splay();
        if(bool(now->left))res += now->left->SubTreeSize;
        return std::make_pair(now ,res);
    }
    
    // insert [NODE]argument into SplayTree (in which nodes are sorted)
    SplayNode *insert_sub(SplayNode *NODE , SplayNode *root , bool paired_compare){
        NODE->update();
        if(bool(root) == false)return NODE;
        // find the border index [x] ( [x]th node [<] [NODE] argument ]
        root = bound_sub(NODE,root,true,paired_compare).first;
        root->eval();
        if(!CompareNode(NODE , root , paired_compare)){
            if(root->right != nullptr)root->right->parent = NODE;
            NODE->right = root->right;
            root->right = nullptr;
            NODE->left = root;
        }else{
            if(root->left != nullptr)root->left->parent = NODE;
            NODE->left = root->left;
            root->left = nullptr;
            NODE->right = root;
        }
        root->parent = NODE;
        root->update();
        NODE->update();
        return NODE;
    }
     
    protected:

    // root node of this tree
    SplayNode *m_Root = nullptr;

    // bluff node object (used as temporary node)
    SplayNode *m_bluff_object = nullptr;
    SplayNode* BluffObject(type_key k , type_value v){
        if(m_bluff_object == nullptr)m_bluff_object = new SplayNode(type_key(0),type_value(0));
        m_bluff_object->Key = k;
        m_bluff_object->Value = v;
        return m_bluff_object;
    }

    // flag of whether node's Values are defined
    // (Values might be undefined if we use insert() function)
    bool _paired = true; 

    void release(){while(m_Root != nullptr)this->Delete(0);}

    void init(){
        m_Root = nullptr;
        _paired = true; 
    }
    
    // pointer of leftmost node
    const SplayNode* const begin(){
        if(size() == 0)return nullptr;
        m_Root = get_sub(0,m_Root);
        return m_Root;
    }

    public:

    MyMultiSet(){init();}
    ~MyMultiSet(){release();}
    // don't copy this object
    MyMultiSet(const MyMultiSet<type_key,type_value> & x) = delete ;
    MyMultiSet& operator = ( const MyMultiSet<type_key,type_value> & x) = delete ;
    MyMultiSet ( MyMultiSet<type_key,type_value>&& x){assert(0);}
    MyMultiSet& operator = ( MyMultiSet<type_key,type_value>&& x){assert(0);}
    // this function makes whole new SplayTree object from [x] argument
    void copy(MyMultiSet<type_key,type_value>& x){
        if(this->begin() == x.begin())return;
        release();
        init();
        for(int i=0;i<x.size();i++){
            SplayNode t=x.get(i);
            this->insert_pair(t.Key,t.Value);
        }
        this->_paired = x._paired;
    }
    
    // tree size
    int size(){
        if(m_Root == nullptr)return 0;
        return m_Root->SubTreeSize;
    }

    // get copy object of [i]th node 
    SplayNode get(int i){
        assert(0 <= i && i < size());
        m_Root = get_sub(i,m_Root);
        SplayNode res = (*m_Root);
        // we cannot access adjacent nodes
        res.right = res.left = res.parent = nullptr;
        return res;
    }

    // get copy object node which covers interval [l,r)
    // Ex. we can get Sum of Key in [l,r)
    SplayNode GetRange(int l , int r){
        assert(0 <= l && l < r && r <= size());
        std::pair<SplayNode*,SplayNode*> tmp = split(r,m_Root);
        SplayNode* rightRoot = tmp.second;
        tmp = split(l,tmp.first);
        SplayNode res = (*tmp.second);
        res.right = res.left = res.parent = nullptr;
        m_Root = merge(merge(tmp.first,tmp.second),rightRoot);
        return res;
    }
 
    // insert key_ 
    void insert( type_key key_ ){
        _paired = false;// undefined Value was added
        m_Root = insert_sub(new SplayNode(key_,type_value(0)) ,m_Root , false);
        return;
    }
 
    // insert (key_ , value_)
    void insert_pair( type_key key_ , type_value val_){
        assert(_paired);
        m_Root = insert_sub(new SplayNode(key_,val_) ,m_Root,true);
        return;
    }
 
    // delete [index]th element
    void Delete(int index){
        assert(0 <= index && index < size());
        std::pair<SplayNode*,SplayNode*> tmp = Delete_sub(index,m_Root);
        m_Root = tmp.first;
        if(tmp.second != nullptr)delete tmp.second;
        return;
    }

    // erase element which has key_ as Key
    void erase(type_key key_){
        int it = find(key_);
        if(it!=-1)Delete(it);
        return;
    }
 
    // erase element which has (key_,value_) as (Key,Value)
    void erase_pair(type_key key_ , type_value val_){
        assert(_paired);
        int it = find_pair(key_ , val_);
        if(it!=-1)Delete(it);
        return;
    }
 
    // counts nodes which < (x)
    int lower_bound(type_key x){
        if(size() == 0)return 0;
        // 比較用 Node オブジェクトを使って比較する
        std::pair<SplayNode*,int> tmp = bound_sub(BluffObject(x,type_value(0)),m_Root,true,false);
        m_Root = tmp.first;
        return tmp.second;
    }

    // counts nodes which < (x,y)
    int lower_bound_pair(type_key x , type_value y){
        assert(_paired);
        if(size() == 0)return 0;
        std::pair<SplayNode*,int> tmp = bound_sub(BluffObject(x,y),m_Root,true,true);
        m_Root = tmp.first;
        return tmp.second;
    }
 
    // counts nodes which <= (x)
    int upper_bound(type_key x){
        if(size() == 0)return 0;
        std::pair<SplayNode*,int> tmp = bound_sub(BluffObject(x,type_value(0)),m_Root,false,false);
        m_Root = tmp.first;
        return tmp.second;
    }
 
    // counts nodes which <= (x,y)
    int upper_bound_pair(type_key x , type_value y){
        assert(_paired);
        if(size() == 0)return 0;
        std::pair<SplayNode*,int> tmp = bound_sub(BluffObject(x,y),m_Root,false,true);
        m_Root = tmp.first;
        return tmp.second;
    }
    
    // find the index [i] which [i]th node has x as Key (if some answer exist,return smallest)
    // if no answer is found, return -1
    int find(type_key x){
        if(size() == 0)return -1;
        if(upper_bound(x) - lower_bound(x) <= 0)return -1;
        return lower_bound(x);
    }

    // find the index [i] which [i]th node has (x,y) as (Key,Value) (if some answer exist,return smallest)
    // if no answer is found, return -1
    int find_pair(type_key x , type_value y){
        assert(_paired);
        if(size() == 0)return -1;
        if(upper_bound_pair(x,y) - lower_bound_pair(x,y) <= 0)return -1;
        return lower_bound_pair(x,y);
    }

 
    SplayNode back(){assert(size()>0);return get(size()-1);}
    SplayNode front(){assert(size()>0);return get(0);}
    void pop_back(){assert(size()>0);Delete(size()-1);}
    void pop_front(){assert(size()>0);Delete(0);}
    SplayNode operator [](int index){return get(index);}               
};
