#pragma OPENCL EXTENSION cl_khr_global_int32_extended_atomics : enable
#pragma OPENCL EXTENSION cl_khr_global_int32_base_atomics : enable
#define BORDERER -1
#define PRAWDA 1
#define FALSZ 0
#define DIFFERENCE_PLATEAU 5

void bords_8(int* borderers_8)
 {
	borderers_8[0] = -1;borderers_8[8]  = -1; /* */ borderers_8[1] = -1;borderers_8[9]  =  0; /* */ borderers_8[2] = -1;borderers_8[10] =  1;
	borderers_8[3] =  0;borderers_8[11] = -1; /* */				/*Piksel*/					  /* */ borderers_8[4] = 0;
	borderers_8[12] =  1;
	borderers_8[5] =  1;borderers_8[13] = -1; /* */ borderers_8[6] =  1;borderers_8[14] =  0; /* */ borderers_8[7] =  1;borderers_8[15] =  1;
 }
 
void bords_cycle_8(int* bords_8)
{
	bords_8[0] = -1; bords_8[9]  = -1; 
	bords_8[1] = -1; bords_8[10] =  0; 
	bords_8[2] = -1; bords_8[11] =  1;
	bords_8[3] =  0; bords_8[12] =  1; 
	bords_8[4] =  1; bords_8[13] =  1;
	bords_8[5] =  1; bords_8[14] =  0;
	bords_8[6] =  1; bords_8[15] = -1;
	bords_8[7] =  0; bords_8[16] = -1;
	bords_8[8] = -1; bords_8[17] = -1;
}

/*
	faza 1
*/
__kernel void watershed_phaseI( __global uchar * input,
								__global int * output,
								   int width,
								   int height)
 {
      int x_position = get_global_id(0);
      int y_position = get_global_id(1);

	  int structure = 8;

	  if(x_position < width && y_position < height){

			int my_value, borderer_value, label, const_label; 
	  
			int borderers[16];
			bords_8(borderers);	
	
			label = mul24(y_position, width) + x_position;
			const_label = label;
			my_value = input[label];
	
		   for(int position = 0; position < structure ; position++){
             int borderer_x = x_position + borderers[position+structure];
             int borderer_y = y_position + borderers[position];
 
			 if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
				  borderer_value = input[mul24(borderer_y,width) + borderer_x];
					if(borderer_value < my_value ){
						my_value = borderer_value;
						label = mul24(borderer_y,width) + borderer_x;
					}
			 }
         }

         output[const_label] = label;
	 }
   
 }

/*
	faza 2
*/

__kernel void watershed_phaseII(  __global int * input,
				__global int * output,
				int width,
				int height)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;

     if(x_position < width && y_position < height){

			int my_position, borderer_value, label; 

			int borderers[16];
			bords_8(borderers);	

			my_position = mul24(y_position, width) + x_position;
			label = input[my_position];
			output[my_position] = label;
	
		if(label == my_position){
			if(x_position == 0 || x_position == width-1 || y_position == 0 || y_position == height-1)
				output[my_position] = BORDERER ;
			else{
				for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_value = mul24(borderer_y,width) + borderer_x ;
							if(borderer_value != input[borderer_value]){
								output[my_position] = BORDERER ;
								break;
							}
						}	
				}
			}
		}	
	 }
 }


/*
	faza 3
*/

 __kernel void watershed_phaseIII( __global int * input,
								   __global int * output,
								   int width,
								   int height)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;

     if(x_position < width && y_position < height){

			int my_position, my_const_position, borderer_value, borderer_lab, label; 

			int borderers[16];
			bords_8(borderers);	

			my_position = mul24(y_position, width) + x_position;
			my_const_position = my_position;
			label = input[my_const_position];
		
		if(label == BORDERER){
				for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_lab = mul24(borderer_y,width) + borderer_x ;
							borderer_value = input[borderer_lab];
							if(borderer_value == BORDERER &&  borderer_lab < my_position)
								my_position = borderer_lab;
						}	
				}
		 output[my_const_position] = my_position;
		}
	 }
 }
 
/*
	faza 4
*/

 __kernel void watershed_phaseIV( __global int * input_lab,
								  __global int * input_lab_lin,
								  __global int * flag,
								  __global int * output,
								  int width,
								  int height)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;


     if(x_position < width && y_position < height){

			int my_position, my_const_position, border_label, label; 
 
			int borderers[16];
			bords_8(borderers);	

			my_position = mul24(y_position, width) + x_position;
			my_const_position = my_position;

			border_label = input_lab[my_const_position];

		if(x_position == 0 && y_position == 0)
			flag[0] = FALSZ;

		if(border_label == BORDERER)
		{
			label = input_lab_lin[my_const_position];
			while(true){
				if(my_position == label){
					output[my_const_position] = my_position;
					break;
				}
				my_position = label;
				label = input_lab_lin[my_position];
			}
		}

	 }
 }

/*
	faza 5
*/

 __kernel   void watershed_phaseV(__global int * input_lab,
								  __global int * input_lab_lin,
								  __global int * flag,
								  __global int * output,
								  int width,
								  int height)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;

     if(x_position < width && y_position < height){

		int my_position, border_label, borderer_value, label; 
 
		int borderers[16];
		bords_8(borderers);	

		my_position = mul24(y_position, width) + x_position;
		border_label = input_lab[my_position];
	
		if(border_label == BORDERER)
		{
			label = input_lab_lin[my_position];
			for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_value = input_lab_lin[mul24(borderer_y,width) + borderer_x]; 
							if(input_lab[mul24(borderer_y,width) + borderer_x] == BORDERER &&  borderer_value < label)
								if(input_lab_lin[label] > input_lab_lin[borderer_value]){
									output[label] =  input_lab_lin[borderer_value];
									flag[0] = PRAWDA;
									break;
							}
						}	
			}
		}
	 }
 }

/*
	faza 6
*/

 __kernel void watershed_phaseVI( __global int * input_lab,
								  __global int * input_lab_lin,
								  __global uchar * input_img,
								  __global int * output,
								  int width,
								  int height)
 {
	 
     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;
     
	 if(x_position < width && y_position < height){
	
			int my_position, label, my_value, border_label, borderer_value, borderer_lab; 
 
			int borderers[16];
			bords_8(borderers);	

		my_position = mul24(y_position, width) + x_position;
		border_label = input_lab[my_position];
		if(border_label == BORDERER)
		{
			label = input_lab_lin[my_position];
			my_value = input_img[my_position];
			for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_lab =  input_lab_lin[mul24(borderer_y,width) + borderer_x];
							borderer_value = input_img[borderer_lab];
							if(borderer_value < my_value && borderer_value > my_value-DIFFERENCE_PLATEAU){
								atom_max(&output[label], borderer_lab);
							}
						}	
			}
		}
	 }
 }

/*
	faza 7
*/

__kernel  void watershed_phaseVII(__global int * input_lab_lin,
								  __global uchar * input_img,
								  __global int * output,
								  int width,
								  int height)
 {
     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;
  
	 if(x_position < width && y_position < height){

			int my_position, my_const_position, label, my_value, borderer_value, borderer_pos; 

			int borderers[16];
			bords_8(borderers);	

			my_position = mul24(y_position, width) + x_position;
			my_const_position = my_position;
			label = input_lab_lin[my_const_position];
			
		if(label == my_position)
		{
			my_value = input_img[my_const_position];
			for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_pos = mul24(borderer_y,width) + borderer_x;
							borderer_value = input_img[borderer_pos];
							if((borderer_value == my_value) && (my_position > borderer_pos))
							{
									my_position = borderer_pos;
							}
						}	
			}
			output[my_const_position] = my_position;
		}
	 }
 }

/*
	faza 8
*/

__kernel   void watershed_phaseVIII(__global int * input_lab_lin,
									__global int * output,
									int width,
									int height)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;

     if(x_position < width && y_position < height){

			int my_position, my_const_position, label, save_point; 
 
			int borderers[16];
			bords_8(borderers);	

			save_point = width + height;

			my_position = mul24(y_position, width) + x_position;
			my_const_position = my_position;
			label = input_lab_lin[my_const_position];

			while(true){
				if(my_position == label || save_point == 0){
					output[my_const_position] = my_position;
					break;
				}
				my_position = label;
				label = input_lab_lin[my_position];
				save_point--;
			}
	 }
 }


__kernel   void watershed_phaseIX( __global int * input_lab_lin,
								  __global int * output,
								  __global int * output_bord,
								  int width,
								  int height,
								  __global int * flag)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 8;

	if(x_position == 0 && y_position == 0)
	{
		flag[0] = FALSZ;
		flag[1] = FALSZ;
		flag[2] = FALSZ;
		flag[3] = FALSZ;
	}

	 if(x_position < width && y_position < height){

			int my_position, my_value, borderer_value; 
	  
			int borderers[16];
			bords_8(borderers);	
	
			my_position = mul24(y_position, width) + x_position;
			my_value = input_lab_lin[my_position];
		
			if(my_position == my_value)
				output_bord[my_position] = my_value;
			else
				output_bord[my_position] = -1;

			//-my_value-1  -- dajemy -1 bo moze trafic sie zero i lepiej mi zabezpieczenie
			if(x_position == 0 || x_position == width-1 || y_position == 0 || y_position == height-1)
				output[my_position] = -my_value-1;
			else{
				for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];
						if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height){
							borderer_value = input_lab_lin[mul24(borderer_y,width) + borderer_x];
							if(borderer_value != my_value ){
								output[my_position] = -my_value-1;
								break;
							}
						}
				}
			}
	 }
 }


__kernel   void watershed_phaseX( __global int * input_lab,
								  __global int * input_lab_lin,
								  __global int * output,
								  __global int * output_border,
								  int width,
								  int height,
								  __global int * flag)
 {

     int x_position = get_global_id(0);
     int y_position = get_global_id(1);

	 int structure = 9;

	 if(x_position < width && y_position < height){
			
			int my_position, my_bord_val ,my_value, borderer_value, borderer_pos, borderer_value_min, temp_where_go, where_go; 
			int is_found = 0;
			bool is_update = true;
	  
			int borderers[18];
			bords_cycle_8(borderers);	
	
			my_position = mul24(y_position, width) + x_position;
			my_bord_val = input_lab[my_position];
			my_value = input_lab_lin[my_position];	
			where_go = my_position;
			

			if(my_bord_val < 0){
				for(int position = 0; position < structure ; position++){
					int borderer_x = x_position + borderers[position+structure];
					int borderer_y = y_position + borderers[position];

					if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height)
					{
						borderer_pos = mul24(borderer_y,width) + borderer_x;
						borderer_value = input_lab_lin[borderer_pos];
						borderer_value_min = input_lab[borderer_pos];

						if(my_value == borderer_value && borderer_value_min < 0 && (is_found == 0 || is_found == 1)){
							temp_where_go=borderer_pos;
							is_found = 1;
						}
						else if( my_value == borderer_value && borderer_value_min >= 0 && is_found == 1){
							where_go = temp_where_go;
							is_found = 2;
						}
						else if(is_found == 2 && my_value != borderer_value)
							is_found = 0;
						else if(is_found != 2)
							is_found = 0;

						if(my_value == borderer_value  && borderer_pos < my_position)
							is_update = false;
					}
					else 
						if(is_found != 2)
								is_found = 0;
			
			/*
					if(borderer_x >=0 && borderer_x < width && borderer_y >=0 && borderer_y < height)
					{
						borderer_pos = mul24(borderer_y,width) + borderer_x;
						borderer_value = input_lab_lin[borderer_pos];
						
							if(my_value != borderer_value && is_found == 0)
								is_found = 1;
							else if(my_value == borderer_value && is_found == 1){
								output[my_position] = borderer_pos;
								is_found = 2;
							}
							else if(my_value == borderer_value && is_found == 2)
								is_found = 3;
							else if(is_found == 2)
								is_found = 0;

							if(my_value == borderer_value && borderer_pos < my_position)
								is_update = false;
					}
					else
						is_found = 1;
*/
				}	
				
				output[my_position] = where_go;

				if(is_update)
					if(input_lab[my_value] > 0) 
						atom_min(&output_border[my_value],my_position);				
		

			}
	 }
 }
