/*
*	���������ù���ģ��
*	���ߣ�������
*	ʱ�䣺2016/03/24
*/

function Config() {
	this.item_json = null;			//��������
	this.hero_json = null;			//Ӣ������
	this.level_json = null;			//��ҵȼ��������
	this.util_json = null;			//�����ۺ�����
	this.shop_json = null;			//�̵�����
	this.vip_json = null;				//vip����
	this.recharge_json = null;		//��ֵ�������
	
	this.init = function() {
		try {
			var item_str = read_json("config/bag/item.json");
			this.item_json = JSON.parse(item_str);
			
			var hero_str = read_json("config/hero/hero.json");
			this.hero_json = JSON.parse(hero_str);
			
			var level_str = read_json("config/player/level.json");
			this.level_json = JSON.parse(level_str);
			
			var util_str = read_json("config/player/util.json");
			this.util_json = JSON.parse(util_str);
			
			var shop_str = read_json("config/shop/shop.json");
			this.shop_json = JSON.parse(shop_str);
			
			var vip_str = read_json("config/vip/vip.json");
			this.vip_json = JSON.parse(vip_str);
			
			var recharge_str = read_json("config/vip/recharge.json");
			this.recharge_json = JSON.parse(recharge_str);
			
		} catch (err) {
			print(err.message);
		}
	}
}
