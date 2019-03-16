import lldb
import lldb.formatters.Logger


class NodeStackSyntheticChildProvider(object):
    def __init__(self, valobj, dict):
        logger = lldb.formatters.Logger.Logger()

        self.valobj = valobj

        self.update()

    def num_children(self):
        logger = lldb.formatters.Logger.Logger()

        if self.len is None:
            self.update()

        return self.len.GetValueAsUnsigned(0)

    def get_child_at_index(self, index):
        logger = lldb.formatters.Logger.Logger()
        logger >> "Retrieving child " + str(index)

        if index < 0 or index >= self.num_children():
            return None

        offset = index * self.data_size

        return self.data.CreateChildAtOffset("[{}]".format(index), offset,
                                             self.data_type)

    def update(self):
        logger = lldb.formatters.Logger.Logger()

        self.data = self.valobj.GetChildMemberWithName('data')
        self.len = self.valobj.GetChildMemberWithName('len')
        self.capacity = self.valobj.GetChildMemberWithName('capacity')
        self.data_type = self.data.GetType().GetPointeeType()
        self.data_size = self.data_type.GetByteSize()

        return True
