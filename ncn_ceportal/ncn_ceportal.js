function ncn_ceportal_bind_ajax_upload(strObj, source_name, file_name, claim_id, claim_file_type){
	var upload_obj = jQuery(strObj);
	var action_url = Drupal.settings.basePath+'ajax_upload/ncn_ceportal/claim_file/'+claim_id+'/'+claim_file_type;
	new AjaxUpload(upload_obj, {
		action: action_url,
		name: source_name,
		data: {	file_name: file_name },
		
		onSubmit: function(file, ext){
			/*if (! (ext && /^(pdf)$/.test(ext))){ 
				// extension is not allowed 
				alert('Only SWF files are allowed');
				return false;
			}*/
			upload_obj.addClass('uploading-file');			
		},
		onComplete: function(file, response){
			upload_obj.removeClass('uploading-file');

			eval("var json = " + response+";");

			//Add uploaded file to list
			
			if(json.flag=="success"){
				//window.location.href=window.location.href;
				window.location.reload();
			}else{
				alert(json.message);
			}
		}
	});
}