package lifegame;
import javax.swing.*;

public class ChangeUndoButton implements BoardListener{
	JButton buttonUndo;
	
	ChangeUndoButton(JButton undoconst){
		buttonUndo=undoconst;
	}
	
	public void updated(BoardModel model2) {
		if(model2.isUndoable()) {
			buttonUndo.setEnabled(true);
		}
		else {
			buttonUndo.setEnabled(false);
		}
	}
}
