// #define	LEN_NAME	256

#define	FS_DOUBLE		sizeof(double)
#define	FS_FLOAT		sizeof(float)
#define	FS_CHAR			sizeof(char)
#define FS_NULL 0

#define	MINUS12345	-12345
#if defined multispec_lin
typedef unsigned short 	UInt16;

typedef signed short 	SInt16;
	
//typedef unsigned long 	UInt32;
	
//typedef signed long 	SInt32; 
typedef SInt16		Boolean;
#define  huge 
typedef double huge *	HDoublePtr;
typedef float huge *	HFloatPtr;
#endif

typedef struct STI_info_str {
	SInt32	no_wave_length_floating_point;
	SInt32	no_V_wave_length_location;	/* no of vertical wave length locations */
	SInt32	no_H_wave_length_location;	/* no of horizontal wave length locations */
	SInt32	*V_wave_length_location;	/* locations of vertical wave length */
	SInt32	*H_wave_length_location;	/* locations of horizontal wave length */ 
	double	*V_wave_length_value;	/* locations of vertical wave length */
	double	*H_wave_length_value;	/* locations of horizontal wave length */ 


	SInt32	col_grid_space;		/* if col_grid_space>0, overwrite the default column
								   grid space. Set zero to use default value */

	SInt32	row_grid_space_flag;
	double	row_grid_space;		/* if row_grid_space_flag>0, overwrite the default row
								   grid space. Set zero to use default value */
	SInt32	row_grid_space_floating_digit;
	

	SInt32	no_col_grid_indicator;	/* put column grids at the user-specified-locations.
									   no_col_grid_indicator = number of desired column grids.
								       Set zero to use default value */
	SInt32	no_col_grid_indicator_floating_point_digit;	/* number of floating digits of the
	                                                       numbers for column grids. */
	SInt32	*no_col_grid_indicator_location;	/* locations of the desired column grid */
	SInt32	*no_col_grid_indicator_value;		/* numbers for the desired column grid 
												   set MINUS12345 if number is not desired */		/* will be used for graph bit map */
	
	SInt32	upper_lower_bound_flag;		/* overwrite the default upper bound and lower bound.
					  					   Set zero to use default value.
					  					   Will be used for graph bit map */
	double	upper_bound;				/* user specified upper bound */
	double	lower_bound;				/* user specified lower bound */
	
	SInt32	mean_min_max_flag;	/* overwrite the default max & min mean.
					  			   Set zero to use default value.
					  			   Will be used for statistics images */	
	double	meanmax_all;		/* user specified max mean */
	double	meanmin_all;		/* user specified min mean */

	SInt32	std_min_max_flag;	/* overwrite the default max & min standard deviation.
					  			   Set zero to use default value.
					  			   Will be used for statistics images */	
	double	stdmax_all;			/* user specified max sd */
	double	stdmin_all;			/* always zero */

	SInt32	mean_min_max_array_flag;				/* set zero to use default value */
	double	*meanmax;								/* NOT available */
	double	*meanmin;		
	
	SInt32	std_min_max_array_flag;					/* set zero to use default value */
	double	*stdmax;								/* NOT available */
	double	*stdmin;
} STI_INFO_STR;

#define	WAVE_LENGTH_DEFAULT_FLAG 123456789

typedef struct class_info_str {
	double				log_det;	
	SInt64				no_sample;	
	UInt32				wave_length_default;
	
	HDoublePtr			data_values;

	HDoublePtr			mean;	
		
	HDoublePtr			cov;
	HDoublePtr			icov;			
	
	unsigned char*		half_image;
	HDoublePtr			fmean;
	HDoublePtr			fvar;
	
	HFloatPtr			mah_dis;
	SInt16				stat_name_storage;	// -1 indicates that this is a selected area.
	Boolean				mah_disLoadedFlag;
	
} CLASS_INFO_STR;