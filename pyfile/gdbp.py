# -*- coding: utf-8 -*-
import gdb.printing

class UnorderedSetPrinter:
    """Pretty-printer for unordered_set."""

    def __init__(self, val):
        self.val = val

    def to_string(self):
        # 获取 unordered_set 类型元素类型的名称
        value_type = str(self.val.type.template_argument(0))
        # 获取 unordered_set 内部哈希表的地址
        table_address = self.val['_M_h'].address
        # 获取哈希表中的桶数（buckets）
        n_buckets = int(table_address['_M_bucket_count'])
        # 获取哈希表中的桶数组（_M_buckets）的地址
        bucket_address = table_address['_M_buckets']
        # 遍历每个桶，并输出桶中所有元素的值
        result = '{'
        for i in range(n_buckets):
            # 获取桶中链表头节点的地址
            list_head_address = bucket_address[i]
            # 如果链表为空，则跳过
            if int(list_head_address) == 0:
                continue
            # 否则，遍历链表并输出元素值
            list_node_address = list_head_address
            while int(list_node_address) != 0:
                # 读取节点值
                node_value = list_node_address.dereference()['_M_v']
                # 将节点值转换成指定类型的指针
                node_value_pointer = node_value.cast(value_type + '*')
                # 输出指针所指向的值
                result += str(node_value_pointer.dereference())
                # 移动到链表的下一个节点
                list_node_address = list_node_address.dereference()['_M_next']
                if int(list_node_address) != 0:
                    result += ', '
        result += '}'
        return result

# 注册 unordered_set pretty-printer
gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    gdb.lookup_type('std::unordered_set<int>').strip_typedefs(),
    UnorderedSetPrinter)