package lifegame;

import java.awt.Graphics;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import javax.swing.JPanel;

public class BoardView extends JPanel implements MouseListener,MouseMotionListener{
	BoardModel model1;
	
	//直前のセル座標を記憶するフィールド
	private int xlast=-1;
	private int ylast=-1;
	//ウィンドウの大きさを記憶するためのフィールド
	private int max;
	
	
	public BoardView() {
		//イベントリスナーの登録
		this.addMouseListener(this);
		this.addMouseMotionListener(this);
	}
	
	public void insertion(BoardModel model2) {
		model1=model2;
	}
	
	//盤面のマス目の多さを記憶するためのフィールド
	
	
	private int square(){
		int block1=1+model1.getCols();
		int xmax=this.getWidth();
		int ymax=this.getHeight();
		if(ymax>xmax) {
			xmax=xmax-xmax%block1;
			return xmax;
		}
		else {
			ymax=ymax-ymax%block1;
			return ymax;
		}
	}
	
	public void mousePressed(MouseEvent e) {
		int block=model1.getCols();
		int block1=block+1;
		max=square()/block1;
		int x=e.getX()/max-1;
		int y=e.getY()/max-1;
		if(x>=0 && x<block && y>=0 && y<block) {
			model1.changeCellState(y,x);
			xlast=x;
			ylast=y;
		}
		this.repaint();
	}
	public void mouseReleased(MouseEvent e) {
		//マウスボタンを話した際に直前の座標をリセット
		xlast=-1;
		ylast=-1;
	}
	public void mouseDragged(MouseEvent e) {
		int block=model1.getCols();
		int block1=block+1;
		max=square()/block1;
		int x=e.getX()/max-1;
		int y=e.getY()/max-1;
		
		//セル座標が有効範囲内でない場合には処理を中断(範囲外におけるエラー表示を防ぐ)
		if (x < 0 || x >= block || y < 0 || y >= block) {
            return;
        }
		// セル座標が有効範囲内で、直前のセルと異なる場合のみ状態を変更
        if (x >= 0 && x < block && y >= 0 && y < block) {
            if (x != xlast || y != ylast) {
                model1.changeCellState(y, x);
                xlast = x;
                ylast = y;
                this.repaint();
                
            }
        }
	}
	
	@Override
	public void paint(Graphics g) {
		int block=model1.getCols();
		int block1=block+1;
		super.paint(g); // JPanel の描画処理（背景塗りつぶし）
		// 直線や塗りつぶしの例
		max=square();
		max=max/block1;
		int i=1;
		for(i=1; i<block1+1; i++) {
			int t=max*i;
			g.drawLine(t, max, t, (max*block1));
			g.drawLine(max, t, (max*block1), t);
		}
		for(i=0; i<block; i++) {
			for(int j=0; j<block; j++) {
				if(model1.getcells(i,j)) {
					g.fillRect(max*(i+1), max*(j+1), max, max);
					
				}
			}
		}
	}

	@Override
	public void mouseClicked(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseEntered(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseExited(MouseEvent arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void mouseMoved(MouseEvent e) {
		// TODO Auto-generated method stub
		
	}

}
