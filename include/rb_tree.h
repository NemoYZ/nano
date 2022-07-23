#pragma once

#include "tree.h"
#include "concepts.h"
#include "type_traits.h"

namespace nano {

enum /*class*/ NodeColor : int8_t {
    BLACK,
    RED,
    WHITE //color of header node
};

struct rb_tree_node_base : public tree_node_base {
    using self 			= rb_tree_node_base;
	using node_base_ptr = self*;

    rb_tree_node_base() noexcept = default;
	rb_tree_node_base(node_base_ptr left, node_base_ptr right) noexcept :
        tree_node_base(left, right) {
    }
    rb_tree_node_base(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent) noexcept :
			tree_node_base(left, right, parent) {
    }

    NodeColor color;
};

template<typename T>
struct rb_tree_node : public rb_tree_node_base {
    rb_tree_node() noexcept = default;

	rb_tree_node(const T& _value) noexcept(std::is_nothrow_assignable_v<T>) :
		value(_value) {
	}

	rb_tree_node(T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
		value(std::move<T>(rvalue)) {
	}
    
	rb_tree_node(node_base_ptr left, node_base_ptr right) noexcept :
        rb_tree_node_base(left, right) {
    }

    rb_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent) noexcept :
			rb_tree_node_base(left, right, parent) {
    }

    rb_tree_node(node_base_ptr left, node_base_ptr right, 
            node_base_ptr parent, const T& _value) noexcept(std::is_nothrow_assignable_v<T>) :
        rb_tree_node_base(left, right, parent),
		value(_value) {
    }

	rb_tree_node(node_base_ptr left, node_base_ptr right,
            node_base_ptr parent, T&& rvalue) noexcept(std::is_nothrow_move_assignable_v<T>) :
        rb_tree_node_base(left, right, parent),
		value(std::move(rvalue)) {
    }

    T value;
};

template<typename T>
NodeColor color_of(const rb_tree_node<T>* node) {
    //空结点是黑色的
	return node ? node->color : NodeColor::BLACK;
}	

template<typename T>
void set_color(rb_tree_node<T>* node, NodeColor color) {
	if (node) {
		node->color = color;
	}
}

template<typename T>
rb_tree_node<T>* rb_insert_fixup(rb_tree_node<T>* node, rb_tree_node<T>** root) {
    while (NodeColor::RED == color_of(parent_of(node))) { //父亲结点为黑色就一直循环
		rb_tree_node<T>* grandparent = parent_of(parent_of(node));
		if (parent_of(node) == left_of(grandparent)) { //父亲结点为祖父结点的左孩子
			rb_tree_node<T>* uncle = right_of(grandparent);
			/**
			* 情况一：叔叔结点为红色
			*		1) 把叔叔和父亲染成黑色
			*		2) 把祖父染成红色
			*		3) 把当前结点设置为祖父结点
			* 情况二：叔叔结点为黑色且当前结点为父亲结点的左孩子
			*		1) 把父亲染为黑色
			*		2) 把祖父染为红色
			*		3) 右旋祖父
			* 情况三：叔叔结点为黑色且当前结点为父亲结点的右孩子
			*		1）左旋父亲结点
			*/
			if (NodeColor::RED == color_of(uncle)) {
				set_color(uncle, NodeColor::BLACK);
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(grandparent, NodeColor::RED);
				node = grandparent;
			} else if (NodeColor::BLACK == color_of(uncle) && 
                    left_of(parent_of(node)) == node) {
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(grandparent, NodeColor::RED);
				right_rotate(grandparent, root);
			} else if (NodeColor::BLACK == color_of(uncle) && 
                    right_of(parent_of(node)) == node) {
				node = parent_of(node);
				left_rotate(node, root);
			}
		} else { //similar to the above
			rb_tree_node<T>* uncle = left_of(grandparent);
			if (NodeColor::RED == color_of(uncle)) {
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(uncle, NodeColor::BLACK);
				set_color(grandparent, NodeColor::RED);
				node = grandparent;
			} else if (NodeColor::BLACK == color_of(uncle) && 
                    right_of(parent_of(node)) == node) {
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(grandparent, NodeColor::RED);
				left_rotate(grandparent, root);
			} else if (NodeColor::BLACK == color_of(uncle) && 
                    left_of(parent_of(node)) == node) {
				node = parent_of(node);
				right_rotate(node, root);
			}
		}
	}
	set_color(*root, NodeColor::BLACK);

	return node;
}

template<typename T>
void rb_erase_fixup(rb_tree_node<T>* node, rb_tree_node<T>** root) {
    while (node != *root && NodeColor::BLACK == color_of(node)) {
		if (node = left_of(parent_of(node))) {
			rb_tree_node<T>* brother = right_of(parent_of(node));
			/**
			* 情况一：兄弟结点为红色
			*		1) 把兄弟结点染为黑色
			*		2) 把父亲结点染为红色
			*		3) 左旋父亲结点
			*		4) 重新赋值brother
			* 情况二：兄弟结点为黑色且兄弟结点的左右孩子都为黑色
			*		1) 把兄弟结点染为红色
			*		2) 把当前结点设置为父亲结点
			* 情况三：兄弟结点为黑色，兄弟结点的左孩子或者右孩子为红色
			*		1) 检查如果兄弟结点的右孩子为黑色则把兄弟染红，把兄弟的右孩子染黑，
			*			右旋兄弟，然后重新设置兄弟结点,剩下就只能为兄弟的右孩子为黑色结点了
			*		2) 把兄弟结点染为父亲结点的颜色
			*		3) 把父亲结点染为黑色
			*		4) 把兄弟结点的右孩子染为红色
			*		5) 左旋父亲结点
			*		6) 把root赋值给node，让其退出循环
			*/
			if (NodeColor::RED == color_of(brother)) {
				set_color(parent_of(node), NodeColor::RED);
				set_color(brother, NodeColor::BLACK);
				left_rotate(parent_of(node), root);
				brother = right_of(parent_of(node));
			} else if (NodeColor::BLACK == color_of(brother) &&
				NodeColor::BLACK == color_of(left_of(brother)) && 
                NodeColor::BLACK == color_of(right_of(node))) {
				set_color(brother, NodeColor::RED);
				node = parent_of(node);
			} else if (NodeColor::BLACK == color_of(brother) &&
				(NodeColor::RED == color_of(left_of(brother)) || 
                NodeColor::RED == color_of(right_of(node)))) {
				if (NodeColor::BLACK == color_of(right_of(brother))) {
					set_color(brother, NodeColor::RED);
					set_color(left_of(brother), NodeColor::BLACK);
					right_rotate(brother, root);
					brother = right_of(parent_of(node));
				}
				set_color(brother, color_of(parent_of(node)));
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(right_of(brother), NodeColor::BLACK);
				left_rotate(parent_of(node), root);
				node = *root;
			}
		} else {
			rb_tree_node<T>* brother = left_of(parent_of(node));
			if (NodeColor::RED == color_of(brother)) {
				set_color(parent_of(node), NodeColor::RED);
				set_color(brother, NodeColor::BLACK);
				right_rotate(parent_of(node), root);
				brother = left_of(parent_of(node));
			} else if (NodeColor::BLACK == color_of(brother) &&
				NodeColor::BLACK == color_of(left_of(brother)) && 
                NodeColor::BLACK == color_of(right_of(node))) {
				set_color(brother, NodeColor::RED);
				node = parent_of(node);
			} else if (NodeColor::BLACK == color_of(brother) &&
				(NodeColor::RED == color_of(left_of(brother)) || 
                NodeColor::RED == color_of(right_of(node)))) {
				if (NodeColor::BLACK == color_of(left_of(brother))) {
					set_color(brother, NodeColor::RED);
					set_color(right_of(brother), NodeColor::BLACK);
					left_rotate(brother, root);
					brother = left_of(parent_of(node));
				}
				set_color(brother, color_of(parent_of(node)));
				set_color(parent_of(node), NodeColor::BLACK);
				set_color(left_of(brother), NodeColor::BLACK);
				right_rotate(parent_of(node), root);
				node = *root;
			}
		}
	}

	set_color(node, NodeColor::BLACK);		//root->color = NodeColor::BLACK
	return node;
}

template<typename T, typename Comp>
std::pair<rb_tree_node<T>*, bool> rb_get_insert_multi(rb_tree_node<T>* root,
        const T& val, const Comp& comp = Comp()) {
    bool insetrLeft = false;
    while (root) {
        if (comp(root->value, val)) {
            insetrLeft = true;
            root = root->left;
        } else {
            insetrLeft = false;
            root = root->right;
        }
    }

    return { root, insetrLeft };
}

template<typename T, typename Comp>
std::pair<rb_tree_node<T>*, int> rb_get_insert_unique(rb_tree_node<T>* root, 
        const T& val, const Comp& comp = Comp()) {
    int insetrLeft = ;
    while (root) {
        if (comp(root->value, val)) {
            insetrLeft = -1;
            root = root->left;
        } else {
            insetrLeft = 1;
            root = root->right;
        }
    }

    return { root, insetrLeft };
}

template<typename T, typename Comp>
rb_tree_node<T>* rb_insert_node_multi(rb_tree_node<T>* node, 
        rb_tree_node<T>** root, const Comp& comp = Comp()) {
    auto [parent, insertLeft] = rb_get_insert_multi(root, node->value, comp);
    node->parent = parent;
    if (insertLeft) {
        parent->left = node;
    } else {
        parent->right = node;
    }

    rb_insert_fixup(node);
    return node;
}

template<typename T, typename Comp>
std::pair<rb_tree_node<T>*, bool> rb_insert_node_unique(rb_tree_node<T>* node, 
        rb_tree_node<T>** root, const Comp& comp = Comp()) {
    auto [parent, insertLeft] = rb_get_insert_unique(*root, node->value, comp);
    switch (insertLeft) {
        case -1:
            parent->left = node;
            break;
        case 1:
            parent->right = node;
            break;
        case 0:
            [[fallthrough]];
        default:
            break;
    }
    node->parent = parent;
    rb_insert_fixup(node);
    return { node, true };
}

/**
 * @brief 红黑树删除节点
 * 
 * @tparam T 
 * @param node 
 * @param root 
 * @return rb_tree_node<T>* 目标删除节点的后继节点
 */
template<typename T>
rb_tree_node<T>* rb_erase_node(rb_tree_node<T>* node, rb_tree_node<T>** root) {
    NodeColor ncolor = color_of(node);
    rb_tree_node<T>* nsuccessor = static_cast<rb_tree_node<T>*>(successor(node));
    rb_tree_node<T>* repNode = nullptr; //replace node

    // 交换要删除的节点和后继节点的位置
    // 转化为只有右孩子的情况
    if (left_of(node) && right_of(node)) { 
        rb_tree_node<T>* sparent = nsuccessor->parent;
		rb_tree_node<T>* srchild = static_cast<rb_tree_node<T>*>(nsuccessor->right);
		nsuccessor->left = node->left;
		node->left->parent = nsuccessor;
		transplant(node, nsuccessor, root);
		node->left = nullptr;

		if (nsuccessor == node->right) {
			node->right = nsuccessor->right;
			if (nsuccessor->right) {
				nsuccessor->right->parent = node;
			}
			nsuccessor->right = node;
			node->parent = nsuccessor;
		} else {
			nsuccessor->right = node->right;
			node->right->parent = nsuccessor;
			node->parent = sparent;
			sparent->left = node;				//_successor肯定是它父亲结点的左孩子
			node->right = srchild;
			if (srchild) {
				srchild->parent = node;
			}
		}
    }

    /**
	* 如果为红色结点，那么就直接删除，不会破坏性质
	* 如果为黑色结点，那就需要调整
	*/
	if (nullptr == left_of(node) && nullptr == right_of(node)) {
		if (node->parent) {
			erase_fixup(node);
			if (node == node->parent->left) {
				node->parent->left = nullptr;
			} else {
				node->parent->right = nullptr;
			}
		}
	} else if (nullptr == left_of(node)) {
		repNode = right_of(node);
		transplant(node, repNode, root);
	} else if (nullptr == right_of(node)) {
		repNode = left_of(node);
		transplant(node, repNode, root);
	}
	
	if (NodeColor::BLACK == ncolor) {
		erase_fixup(replace_node);
	}
	(*root)->color = NodeColor::BLACK;
	
	return nsuccessor;
}

template<typename T, typename Comp>
rb_tree_node<T>* rb_lbound(const T& val, rb_tree_node<T>* root, const Comp& comp = Comp()) {
    rb_tree_node<T>* parent = nullptr;
    while (root) {
        parent = root;
        if (!comp(root->value, val)) {
            root = root->left;
        } else {
            root = root->right;
        }
    }

    return parent;
}

template<typename T>
rb_tree_node<T>* rb_ubound(const T& val, rb_tree_node<T>* root) {
    rb_tree_node<T>* parent = nullptr;
    while (root) {
        parent = root;
        if (comp(val, root->value)) {
            root = root->left;
        } else {
            root = root->right;
        }
    }

    return parent;
}

template<typename T>
std::pair<rb_tree_node<T>*, rb_tree_node<T>*> rb_equal_range_multi(const T& val, rb_tree_node<T>* root) {
    return { rb_lbound(val, root), rb_ubound(val, root) };
}

template<typename T>
std::pair<rb_tree_node<T>*, rb_tree_node<T>*> rb_equal_range_unique(const T& val, rb_tree_node<T>* root) {
    rb_tree_node<T>* node = rb_lbound(val, root);
    return { node, static_cast<rb_tree_node<T>*>(successor(node)) };
}

} //namespace nano