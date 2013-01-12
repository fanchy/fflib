from pylib.inc import *

class src_parser_t:
    def __init__(self, file):
        self.file = file
        self.struct_def_mgr = struct_def_mgr_t()
        self.file_content = ''
        self.all_words    = []
        f = open(file)
        self.file_content = f.read()
        f.close()
    def get_struct_def_mgr(self):
        return self.struct_def_mgr
    def parse_to_words(self):
        all_line = self.file_content.split('\n')
        for line in all_line:
            words = line.split(' ')
            for w in words:
                w = w.strip()
                if w != '':
                    self.all_words.append(w)

    def build_struct_relation(self):
        struct_stack = []
        index = 0
        while index < len(self.all_words):
            if len(struct_stack) < 1:
                struct_stack.append(self.struct_def_mgr)

            parent_struct = struct_stack[len(struct_stack) - 1]
            cur_word = self.all_words[index]
            if cur_word == 'struct':
                struct_def = struct_def_t(self.all_words[index + 1])
                parent_struct.add_struct(struct_def)
                struct_stack.append(struct_def)
                index = index + 1
            elif cur_word == '}' or cur_word == '};':
                struct_stack.pop()
            elif cur_word == 'int8' or cur_word == 'int16' or cur_word == 'int32' or \
               cur_word == 'float' or cur_word == 'string':
                field_name = self.all_words[index + 1].split(';')[0]
                field = field_def_t(field_name, cur_word, '', '')
                parent_struct.add_field(field)
                index = index + 1
            else:
                if -1 == cur_word.find('dictionary') and  -1 == cur_word.find('{') and -1 == cur_word.find('array') :
                    field_name = self.all_words[index + 1].split(';')[0]
                    field = field_def_t(field_name, cur_word, '', '')
                    parent_struct.add_field(field)
                    index = index + 1
                else:
                    field_type = ''
                    field_name = ''
                    key_type   = ''
                    val_type   = ''
                    if -1 != cur_word.find('array'):
                        field_name = self.all_words[index + 1].split(';')[0]
                        word_split = cur_word.split('<')
                        field_type = word_split[0]
                        key_type = word_split[1].split('>')[0]
                        field = field_def_t(field_name, field_type, key_type, '')
                        parent_struct.add_field(field)
                        index = index + 1
                    elif -1 != cur_word.find('dictionary'):
                        field_name = self.all_words[index + 1].split(';')[0]
                        word_split = cur_word.split('<')
                        field_type = word_split[0]
                        key_val_type = word_split[1].split('>')
                        key_type = key_val_type[0].split(',')[0]
                        val_type = key_val_type[0].split(',')[1]
                        field = field_def_t(field_name, field_type, key_type, val_type)
                        parent_struct.add_field(field)
                        index = index + 1
            index = index + 1
            
    def exe(self):
        self.parse_to_words()
        self.build_struct_relation()
        

