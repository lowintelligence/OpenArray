
///:include "../../NodeType.fypp"
#include "../../NodePool.hpp"
#include "../../Grid.hpp"

namespace oa{
  namespace ops{

    ///:for o in [i for i in L if i[3] == 'D']
    ///:set type = o[0]
    ///:set name = o[1]
    NodePtr new_node_${name}$(const NodePtr& u){
      NodePtr np = NodePool::global()->get();
      np->set_type(${type}$);
      np->add_input(0, u);
      
      int dt = u->get_data_type();

      // only OP will change grid pos
      np->set_pos(u->get_pos());

      np->set_depth(u->get_depth());
      np->set_shape(u->shape());

      np->set_data_type(dt);
      np->set_lbound(u->get_lbound());
      np->set_rbound(u->get_rbound());
        
      int3 lb, rb;
    
      ///:if name in ['axb', 'dxb']
      lb = {1, 0, 0};
      rb = {0, 0, 0};
      ///:elif name in ['axf', 'dxf']
      lb = {0, 0, 0};
      rb = {1, 0, 0};
      ///:elif name in ['ayb', 'dyb']
      lb = {0, 1, 0};
      rb = {0, 0, 0};
      ///:elif name in ['ayf', 'dyf']
      lb = {0, 0, 0};
      rb = {0, 1, 0};
      ///:elif name in ['azb', 'dzb']
      lb = {0, 0, 1};
      rb = {0, 0, 0};
      ///:elif name in ['azf', 'dzf']
      lb = {0, 0, 0};
      rb = {0, 0, 1};
      ///:elif name in ['dxc']
      lb = {1, 0, 0};
      rb = {1, 0, 0};
      ///:elif name in ['dyc']
      lb = {0, 1, 0};
      rb = {0, 1, 1};
      ///:elif name in ['dzc']
      lb = {0, 0, 1};
      rb = {0, 0, 1};  
      ///:endif

      int3 new_lb = u->get_lbound();
      int3 new_rb = u->get_rbound();
          
      int mx = 0;
      for (int i = 0; i < 3; i++) {
        new_lb[i] += lb[i];
        mx = max(new_lb[i], mx);
        new_rb[i] += rb[i];
        mx = max(new_rb[i], mx);
      }

      // set default max stencil as two
      if (mx > 1) {
        np->set_lbound(lb);
        np->set_rbound(rb);
        u->set_update();
      } else {
        np->set_lbound(new_lb);
        np->set_rbound(new_rb);
      }

      if(u->get_pos() != -1){
        np->set_pos(Grid::global()->get_pos(u->get_pos(), ${type}$));
      }

      np->set_pseudo(u->is_pseudo());

      np->set_bitset(u->get_bitset());
  
      return np;
    }
    ///:endfor

  }
}
