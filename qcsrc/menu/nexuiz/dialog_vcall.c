#ifdef INTERFACE
CLASS(NexuizVcallDialog) EXTENDS(NexuizRootDialog)
	METHOD(NexuizVcallDialog, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	ATTRIB(NexuizVcallDialog, title, string, "Call a vote")
	ATTRIB(NexuizVcallDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	ATTRIB(NexuizVcallDialog, intendedWidth, float, 0.8)
	ATTRIB(NexuizVcallDialog, rows, float, 16)
	ATTRIB(NexuizVcallDialog, columns, float, 20)
	ATTRIB(NexuizVcallDialog, name, string, "VcallMenu")
    ATTRIB(NexuizVcallDialog, voteList, entity, NULL)
ENDCLASS(NexuizVcallDialog)
#endif

#ifdef IMPLEMENTATION

void VoteCall_f(entity btn, entity me)
{
    localcmd("cmd vote call ", CVAR_STR(_menu_vote_string), "\n");
    btn.onClickEntity.close(btn.onClickEntity);
}

void fillNexuizVcallDialog(entity me)
{
	entity e, vlist;
	
	me.TR(me);
		me.TD(me, 1, 3, e = makeNexuizTextLabel(0, "Available votes:"));
	me.TR(me);
        me.TD(me, 13, 20, vlist = makeNexuizVoteList());
	me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me); // ???
        me.TD(me, 1, 20, e = makeNexuizInputBox(1, "_menu_vote_string"));
        //e.saveImmediately = 1;
        vlist.textBox = e;
    me.TR(me);
        me.TD(me, 1, 10, e = makeNexuizButton("Call the vote", '0 0 0'));
            e.onClick = VoteCall_f;
            e.onClickEntity = me;
        me.TD(me, 1, 10, e = makeNexuizCommandButton("Cancel", '0 0 0', "", 1));
    
    me.voteList = vlist;
}
#endif

/* Click. The c-word is here so you can grep for it :-) */

