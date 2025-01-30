package lifegame;
import java.util.ArrayList;
import java.util.*;

public class BoardModel {
	private int cols;
	private int rows;
	private ArrayList<BoardListener> listeners;
	private LinkedList<Boolean[][]> history;
	
	
	private boolean[][] cells;
	private boolean[][] curcell;
	
	
	
	
	public BoardModel(int c, int r) {
		cols = c;
		rows = r;
		cells=new boolean[rows][cols];
		curcell=new boolean[rows][cols];
		listeners = new ArrayList<BoardListener>();
		history =new LinkedList<Boolean[][]>();
	}
	
	public void addListener(BoardListener listener) {
		listeners.add(listener);
	}
	private void fireUpdate() {
		for(BoardListener listener: listeners) {
			listener.updated(this);
		}
	}
	public int getCols() {return cols;}
	public int getRows() {return rows;}
	
	public boolean getcells(int i,int j) {
		return cells[i][j];
	}
	

	public void changeCellState(int y, int x) {
		// (x, y) で指定されたセルの状態を変更する。
		save();
		if(cells[x][y]) {
			cells[x][y]=false;
		}
		else {
			cells[x][y]=true;
		}
		this.fireUpdate();
	}
	
	private int countlive (int row, int col) {
		int count=0;
		for(int i=-1; i<=1; i++) {
			for(int j=-1; j<=1; j++) {
				if(i!=0 || j!=0) {
					int tate=row+i;
					int yoko=col+j;
					if(tate >=0 && tate<cols) {
						if(yoko>=0 && yoko<rows) {
							if(curcell[tate][yoko]) {
								count++;
							}
						}
					}
				}
			}
		}
		return count;
	}
	
	public void next() {
		//盤面の状態をライフゲームのルールに従って１世代更新する。
		save();
		int i,j;
		for(i=0; i<rows; i++) {
			for(j=0; j<cols; j++) {
	
				int nextlive=countlive(i,j);
				if((nextlive==3) || (nextlive==2 && curcell[i][j])) {
					cells[i][j]=true;
				}
				else {
					cells[i][j]=false;
				}
				//changeCellState(i,j);
			}
		}
		System.out.println();
		
		
		this.fireUpdate();
	}
	
	public void save() {
		
		int i,j;
		Boolean[][] savecell=new Boolean[rows][cols];
		for(i=0; i<rows; i++) {
			for(j=0; j<cols; j++) {
				curcell[i][j]=cells[i][j];
				savecell[i][j]=cells[i][j];
			}
		}
		if(history.size()>=32) {
			history.removeLast();
		}
		history.push(savecell);
		
	}
	public void undo() {
		Boolean[][] undocell=new Boolean[rows][cols];
		int i,j;
		if(history.size()!=0) {
			undocell=history.pop();
			for(i=0; i<rows; i++) {
				for(j=0; j<cols; j++) {
					cells[i][j]=undocell[i][j];
				}
			}
			if(history.size()>1) {
				undocell=history.get(history.size()-1);
				for(i=0; i<rows; i++) {
					for(j=0; j<cols; j++) {
						curcell[i][j]=undocell[i][j];
					}
				}
			}
		}
		this.fireUpdate();
	}
	
	public void allclear() {
		int i,j;
		for(i=0; i<rows; i++) {
			for(j=0; j<cols; j++) {
				cells[i][j]=false;
			}
		}
		this.fireUpdate();
	}
	public boolean isUndoable() {
		if(history.size()>=1) {
			return true;
		}
		else {
			return false;
		}
	}
	
	
	
}

	
