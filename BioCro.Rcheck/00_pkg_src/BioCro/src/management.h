struct planting
{
  int dayofyear;
  double rate; // in Mg/ha
};

struct harvesting
{
  struct input{
              int numberofharvest;
              double *dayofyear;
              double *stemfractionleft;
              double *leaffractionleft;
              double *stemlitterleft;
              };
  struct output{
              double *stem;
              double *leaf;
              double *rhizome;
              double *root;
                };
};

struct Napplication 
                   {
                    int numberofNapplication;
                    double *dayofyear;
                    double rate;
                    char method;
                    char type;
                    };
                    
struct management
                {
                  struct planting planting;
                  struct harvesting harvesting;
                  struct Napplication Napplication;
                };
                
