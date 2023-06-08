
/**
 * @file
 * @author Andrew Ciezak <andy@impv.au>
 * @version 1.0
 * @brief Header for the DataSet class
 * 
 * Adapted from work by @author Rui Zhang <zhang_ray@stu.scu.edu.cn> from https://link.springer.com/chapter/10.1007/978-3-031-20862-1_38
*/

#ifndef CUSR_TREE_NODE_H
#define CUSR_TREE_NODE_H

namespace cusr {

    /** Types of nodes, such as variables, constants and functions */
    typedef enum NodeType {
        VAR,        // variable
        CONST,      // constant
        UFUNC,      // unary function
        BFUNC       // binary function
    } ntype_t;

    /** For Nodes of a function NodeType, what operation is applied to their child (uniary) or childs (binary) */
    typedef enum Function {
        ADD,        // arity: 2, return a + b
        SUB,        // arity: 2, return a - b
        MUL,        // arity: 2, return a * b
        DIV,        // arity: 2, if (b == 0) { b = DELTA } return a / b
        TAN,        // arity: 1, return tan a
        SIN,        // arity: 1, return sin a
        COS,        // arity: 1, return cos a
        LOG,        // arity: 1, return log a
        MAX,        // arity: 2, if (a > b) { return a } return b
        MIN,        // arity: 2, if (a < b) { return a } return b
        INV         // arity: 1, if (a == 0) { a = DELTA } return 1 / a
    } func_t;
    
    /** A Node with a NodeType and associated variable, constant or function*/
    struct Node {
        ntype_t node_type;  // type of the node
        double constant;     // value of constant
        int variable;       // the number of variable (e.g., 0 refers to x0; 1 refers to x1).
        func_t function;    // type of function
    };

    /** A prefix is the flatted Node list from the binary tree containing TreeNode structures that is easier to iterate */
    typedef std::vector<Node> prefix_t;

    /** Comparison operator for Node */
    inline bool operator==(const Node& lhs, const Node& rhs)
    {
        if( lhs.node_type != rhs.node_type )        
            return false;

        if( lhs.node_type == NodeType::BFUNC || lhs.node_type == NodeType::UFUNC ) {
            if( lhs.function != rhs.function )       
                return false;
        }

        if( lhs.node_type == NodeType::CONST ) {
            if( lhs.constant != rhs.constant )      
                return false;
        }
        
        if( lhs.node_type == NodeType::VAR) {
            if( lhs.variable != rhs.variable )      
                return false;
        }

        return true;
    }

    /** A Node in the context of a binary tree. Contains the current Node and left/right Nodes */
    struct TreeNode {
        TreeNode() : left(nullptr), right(nullptr) {}
        ~TreeNode() {
            if(left != nullptr) {
                delete left;
                left = nullptr;
            }
            if(right != nullptr) {
                delete right;
                right = nullptr;
            }
        } 
        Node node;
        TreeNode *left;
        TreeNode *right;
    };

    /** Comparison operator for TreeNode */
    inline bool operator==(const TreeNode& lhs, const TreeNode& rhs)
    {
        if( !(lhs.node == rhs.node) )       
            return false;

        if( lhs.node.node_type == NodeType::BFUNC ) {
            if( lhs.left == nullptr )           
                return false;
            if( lhs.right == nullptr )          
                return false;
            if( !(*lhs.left == *rhs.left) )     
                return false;
            if( !(*lhs.right == *rhs.right) )   
                return false;
        }

        if( lhs.node.node_type == NodeType::UFUNC ) {
            if( lhs.left == nullptr )           
                return false;
            if( !(*lhs.left == *rhs.left) )     
                return false;
            if( lhs.right != nullptr )          
                return false;
        }

        return true;
    }

    inline void get_prefix(prefix_t &prefix, TreeNode *tree_node) {
        if (tree_node == nullptr) {
            return;
        }
        prefix.emplace_back(tree_node->node);
        get_prefix(prefix, tree_node->left);
        get_prefix(prefix, tree_node->right);
    }

    static string function_to_string(Function function) {
        switch (function) {
            case Function::ADD:
                return "+";

            case Function::SUB:
                return "-";

            case Function::MUL:
                return "*";

            case Function::DIV:
                return "/";

            case Function::MAX:
                return "max";

            case Function::MIN:
                return "min";

            case Function::SIN:
                return "sin";

            case Function::COS:
                return "cos";

            case Function::TAN:
                return "tan";

            case Function::LOG:
                return "log";

            case Function::INV:
                return "inv";

            default:
                return "error";
        }
    }

    inline string prefix_to_infix(prefix_t &prefix) {
        stack<string> s;
        for (int i = prefix.size() - 1; i >= 0; i--) {
            Node &node = prefix[i];
            if (node.node_type == NodeType::CONST) {
                s.push(std::to_string(node.constant));
            } else if (node.node_type == NodeType::VAR) {
                string var = "x";
                var.append(std::to_string(node.variable));
                s.push(var);
            } else if (node.node_type == NodeType::BFUNC) {
                string tmp = "(";
                tmp.append(s.top()).append(" ");
                s.pop();
                tmp.append(function_to_string(node.function));
                tmp.append(" ").append(s.top()).append(")");
                s.pop();
                s.push(tmp);
            } else {
                string tmp;
                tmp = function_to_string(node.function);
                if (s.top().size() == 1) {
                    tmp.append(s.top());
                } else {
                    tmp.append("(").append(s.top()).append(")");
                }
                s.pop();
                s.push(tmp);
            }
        }
        return s.top();
    }
}

#endif