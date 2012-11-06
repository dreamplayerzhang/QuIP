
#include "quip_config.h"

char VersionId_opengl_tilemenu[] = QUIP_VERSION_STRING;

#ifdef HAVE_OPENGL

#include "tile.h"
#include "data_obj.h"

List *tile_lp=NO_LIST;

#ifdef DEBUG
#include "debug.h"
u_long debug_tiles=0;
#endif /* DEBUG */

/* TILE_ITERATE executes the statement on all master tiles in the list */

#define TILE_ITERATE( statement )					\
									\
	if ( tile_lp == NO_LIST ){					\
		NWARN("no tiles");					\
		return;							\
	}								\
									\
	np = tile_lp->l_head;						\
	while(np!=NO_NODE){						\
		statement;						\
		np = np->n_next;					\
	}

#define THIS_TILE	(((Master_Tile *)np->n_data)->mt_tp)

static COMMAND_FUNC( do_add )
{
	float x,y;
	Vertex *vp[4];
	Master_Tile *mtp;
	int i;
	char prompt[128];
	Node *np;
	const char *s;

	s=NAMEOF("elevation map stem");

	for(i=0;i<4;i++){
		sprintf(prompt,"x coordinate of %s vertex",quad_name[i]);
		x = HOW_MUCH(prompt);
		sprintf(prompt,"y coordinate of %s vertex",quad_name[i]);
		y = HOW_MUCH(prompt);

		vp[i] = new_vertex(x,y);
	}
	mtp = new_master_tile(vp[0],vp[1],vp[2],vp[3]);

	mtp->mt_dem_name=savestr(s);	/* save the height map */

	/* the dem_name is just the quad name, so we use it for the texture name too... */
	mtp->mt_tex_name = savestr( mtp->mt_dem_name );

	if( tile_lp == NO_LIST )
		tile_lp = new_list();
#ifdef CAUTIOUS
	if( tile_lp == NO_LIST )
		ERROR1("CAUTIOUS:  error creating tile list");
#endif /* CAUTIOUS */
	np = mk_node(mtp);
	addTail(tile_lp,np);

advise("elevating master tile");
	master_tile_elevate(QSP_ARG  mtp);
advise("texturing master tile");
	master_tile_texture(QSP_ARG  mtp);
}

static COMMAND_FUNC( do_show )
{
	Node *np;
	TILE_ITERATE( show_tile( THIS_TILE,"") )
}

static COMMAND_FUNC( do_xdraw_tiles )
{
	Node *np;

	TILE_ITERATE( xdraw_master( ((Master_Tile *)np->n_data) ) )
	TILE_ITERATE( undivide_tile( THIS_TILE ) )
}

static COMMAND_FUNC( do_draw_tiles )
{
	Node *np;

	TILE_ITERATE( draw_tile( THIS_TILE ) )
}

static COMMAND_FUNC( do_xform_tiles )
{
	Data_Obj *dp;
	Node *np;

	dp = PICK_OBJ("transformation matrix");
	if( dp == NO_OBJ ) return;

	/* We call tile_check_subdiv to determine the subdivisions */
	TILE_ITERATE( tile_check_subdiv( THIS_TILE,dp) )

	/* These are not view-specific, and should be done when the master tiles
	 * are created...
	 */

#ifdef FOOBAR
advise("setting tile elevations");
	TILE_ITERATE( master_tile_elevate(np->n_data) )
	TILE_ITERATE( master_tile_texture(np->n_data) )
	/* set the texture for the master tiles - only needs to be done once! */
#endif /* FOOBAR */
}

static COMMAND_FUNC( do_set_bb )
{
	float xl,xr,yb,yt;

	xl=HOW_MUCH("left x limit");
	yb=HOW_MUCH("bottom y limit");
	xr=HOW_MUCH("right x limit");
	yt=HOW_MUCH("top y limit");

	set_coord_limits(xl,yb,xr,yt);
}

static COMMAND_FUNC( do_tile_info )
{
#ifdef HASH_TILE_NAMES
	Tile *tp;

	tp = PICK_TILE("tile name");
	tile_info(tp);
#else
	const char *s;

	s=NAMEOF("tile name");
	advise("Sorry, need to compile with HASH_TILE_NAMES defined to do lookup by name");
#endif
}

static COMMAND_FUNC( do_dem_dir )
{
	dem_directory = savestr( NAMEOF("DEM directory") );
}

static COMMAND_FUNC( do_tex_dir )
{
	tex_directory = savestr( NAMEOF("texture directory") );
}

#ifndef HASH_TILE_NAMES
static COMMAND_FUNC( list_tiles )
{
	advise("Sorry, need to compile with HASH_TILE_NAMES defined to list all tile names");
}
#endif /* HASH_TILE_NAMES */

static COMMAND_FUNC( do_set_dthresh )
{
	float d;

	d=HOW_MUCH("distance threshold");
	set_dthresh(d);
}

static Command tile_ctbl[]={
{ "add",	do_add,		"add a tile"				},
{ "show",	do_show,	"show all tiles"			},
{ "draw",	do_draw_tiles,	"draw tiles"				},
{ "xdraw",	do_xdraw_tiles,	"draw transformed tiles"		},
{ "xform",	do_xform_tiles,	"transform tiles, subdividing as necessary"},
{ "threshold",	do_set_dthresh,	"set threshold distance for tile subdivision"},
{ "bounding_box",do_set_bb,	"specify bounding box for drawing"	},
{ "info",	do_tile_info,	"print info about current tile"		},
{ "list",	list_tiles,	"list names of all tiles"		},
{ "dem_directory",do_dem_dir,	"specify DEM directory"			},
{ "tex_directory",do_tex_dir,	"specify texture directory"		},
{ "quit",	popcmd,		"exit submenu"				},
{ NULL_COMMAND								}
};


COMMAND_FUNC( tile_menu )
{
	static int inited=0;

	if( !inited ){
		init_dir_names();
		inited=1;
#ifdef DEBUG
		debug_tiles = add_debug_module(QSP_ARG  "tiles");
#endif /* DEBUG */
	}
	PUSHCMD(tile_ctbl,"tiles");
}

#endif /* HAVE_OPENGL */
