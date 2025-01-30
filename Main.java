package lifegame;

import javax.swing.*;
import java.awt.BorderLayout;
import java.awt.event.*;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class Main implements Runnable{
	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Main());
	}
	
	public void run() {
		String value = JOptionPane.showInputDialog(null, "盤面の大きさを100以下の整数で入力してください");
		int size=Integer.parseInt(value.trim());
		// BoardModel の作成と changeCellState の呼び出しを行う処理をここで実行。
		BoardModel model2=new BoardModel(size,size);
		BoardView view = new BoardView();
		view.insertion(model2);
		
		// ウィンドウを作成する
		JFrame frame = new JFrame("lifegame");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		// ウィンドウ内部を占有する「ベース」パネルを作成する
		JPanel base = new JPanel();
		frame.setContentPane(base);
		base.setPreferredSize(new Dimension(800, 800)); // 希望サイズの指定
		frame.setMinimumSize(new Dimension(450, 450));
		
		base.setLayout(new BorderLayout());
		
		
		
		
		// base 上に配置する GUI 部品のルールを設定
		
		JPanel buttonPanel = new JPanel(); // ボタン用パネルを作成し
		base.add(buttonPanel, BorderLayout.SOUTH); // base の下端に配置する
		buttonPanel.setLayout(new FlowLayout()); // java.awt.FlowLayout を設定
		JButton nextbutton = new JButton("next");
		JButton undobutton = new JButton("undo");
		
		//mainメソッド内にボタンを押したらnext実行されるメソッド
		buttonPanel.add(nextbutton);
		buttonPanel.add(undobutton);
		//nextボタンクリック処理
		//最初に盤面をセットする(これはあとで記述する)
		nextbutton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				model2.next();
				//next実行後にundoが可能か再判定し、ボタンを有効に修正する。
				//ボタンを押したらundo実行されるメソッド
				undobutton.setEnabled(model2.isUndoable());
				//画面表示の実行
				view.insertion(model2);
				view.repaint();
			}
		});
		//初期状態でundoボタンを無効化しておく
		undobutton.setEnabled(model2.isUndoable());
		BoardListener listener = new ChangeUndoButton(undobutton);
		model2.addListener(listener);
		
		//undoボタンクリック処理
		undobutton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				model2.undo();
				//undo実行後にundoが可能か再判定し、不可能であるならばボタンを無効に修正する。
				model2.addListener(listener);
				//画面表示の実行
				view.insertion(model2);
				view.repaint();
			}
		});
		
		JButton allbutton = new JButton("allclear");
		buttonPanel.add(allbutton);
		allbutton.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				model2.next();
				model2.allclear();
				//allclear実行後にundoボタンを無効に修正する。
				model2.addListener(listener);
				//サイズ変更を可能にする
				//画面表示の実行
				view.insertion(model2);
				view.repaint();
				
			}
		});
	
		
		
		//new gameボタンの設定
		JButton newbutton = new JButton("new game");
		buttonPanel.add(newbutton);
		newbutton.addActionListener(new ActionListener(){
			@Override
			public void actionPerformed(ActionEvent e) {
				SwingUtilities.invokeLater(new Main());
			}
		});
		
		base.add(view, BorderLayout.CENTER);//baseの中央にviewを配置する
		frame.pack();
		frame.setVisible(true); // ウィンドウを表示する
		// 最小サイズの指定
		// ウィンドウに乗せたものの配置を確定する
	
	}
}
