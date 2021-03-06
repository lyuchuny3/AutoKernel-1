/*

step 1: init_node
        init the private info data for your op, if no need, skip this
step 2: prerun
        allocate helper memory for your op, if no need, skip this
step 3: run
        complete the run function to use the function generated by autokernel
step 4: postrun
        release helper memory you allocated for your op, if no need, skip this
step 5: release_node
        release the private info data you allocated for your op, if no need, skip this
step 6: score
        adjust you score priority strategy, default score value is defined in cpu_node_ops.h
step 7: reshape
        reshape output tensor, if no need, skip this
step 8: register op
        change register func name and called in init.cpp
*/

#include "softmax.h"

// add helper data struct and functions here
/*
struct op_priv_info
{

};
*/

static int prerun(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    /*
    allocate helper memory for your op
    */
    return 0;
}

static int run(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    bool info_autokernel = false;
    const char* debug_env = std::getenv("DEBUG_INFO");
    if((debug_env) && (debug_env[0] == '1'))
    {
        info_autokernel = true;
    }
    // step 1: get input and output
    struct ir_node* ir_node = exec_node->ir_node;
    struct ir_graph* ir_graph = ir_node->graph;
    struct ir_tensor* input_tensor = get_ir_graph_tensor(ir_graph, ir_node->input_tensors[0]);
    struct ir_tensor* output_tensor = get_ir_graph_tensor(ir_graph, ir_node->output_tensors[0]);

    // get op private data info (if needed)
    // struct op_priv_info* conv_priv_info = ( struct conv_priv_info* )exec_node->ops_priv;

// struct softmax_param* softmax_param=(strcut softmax_param*)ir_node->op.param_mem;

    // step 2: get op params (if needed), the op_param struct is defined in src/op/
    // struct op_param* op_param = ( struct conv_param* )ir_node->op.param_mem;
    // DTYPE [param_list] = conv_param->param_list;

    // step 3: call the func generated by Autokernel
    Halide::Runtime::Buffer<float> input((float*)input_tensor->data, input_tensor->dims[1], input_tensor->dims[0]);
    Halide::Runtime::Buffer<float> output((float*)output_tensor->data,  output_tensor->dims[1], output_tensor->dims[0]);

    halide_softmax(input, output);

    if(info_autokernel)printf("[INFO]: runing Autokernel halide_softmax...\n");
    return 0;
}

static int reshape(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    return 0;
}

static int postrun(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    /*
    release the helper memory you 
    */
    return 0;
}

static int init_node(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    /* 
    init the private info data for your op:
    void ops_priv;
    int shared_mem_size;
    int shared_pack4_mem_size;
    */
    return 0;
}

static int release_node(struct node_ops* node_ops, struct exec_node* exec_node, struct exec_graph* exec_graph)
{
    /* 
    release the private info data for your op:
    void ops_priv;
    int shared_mem_size;
    int shared_pack4_mem_size;
    */
    return 0;
}

static int score(struct node_ops* node_ops, struct exec_graph* exec_graph, struct ir_node* exec_node)
{
    /*
    OPS_SCORE_STATIC 10000
    OPS_SCORE_BEST 8000
    OPS_SCORE_PREFER 6000
    OPS_SCORE_CANDO 4000
    OPS_SCORE_NOTSUP 2000
    */
    return OPS_SCORE_STATIC;
}

static struct node_ops autokernel_node_ops = {.prerun = prerun,
                                       .run = run,
                                       .reshape = reshape,
                                       .postrun = postrun,
                                       .init_node = init_node,
                                       .release_node = release_node,
                                       .score = score};

static int reg_autokernel_ops(void* arg)
{
    return register_builtin_node_ops(OP_SOFTMAX, &autokernel_node_ops);
}

//static int unreg_autokernel_ops(void* arg)
//{
//    unregister_builtin_node_ops(OP_SOFTMAX, &autokernel_node_ops);
//    return 0;
//}

void RegisterAutoKernelSoftmax()
{
    register_norm_module_init(2, "reg_autokernel_ops", reg_autokernel_ops, NULL);
}

